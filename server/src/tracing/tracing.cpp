#include "tracing/tracing.h"

#include <spdlog/spdlog.h>

#include <fstream>

// =============================================================================
// PERFETTO_TRACK_EVENT_STATIC_STORAGE
// =============================================================================
// PERFETTO_DEFINE_CATEGORIES (헤더에서 선언)에 대응하는 정적 저장소.
// 프로젝트 전체에서 정확히 하나의 .cpp 파일에만 이 매크로가 있어야 한다.
// 여러 .cpp에 넣으면 링크 에러(중복 심볼)가 발생한다.
PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace psh::tracing {

// ---------------------------------------------------------------------------
// 내부 상태
// ---------------------------------------------------------------------------
// TracingSession은 Perfetto의 트레이스 수집 단위.
// 하나의 세션이 활성화되면 설정된 카테고리의 이벤트가 내부 버퍼에 기록된다.
// 세션을 Stop하면 버퍼의 데이터를 꺼내서 파일로 저장할 수 있다.
static std::unique_ptr<perfetto::TracingSession> g_session;

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------
void Init() {
    // TracingInitArgs: Perfetto 초기화 파라미터
    //
    // backends 필드에 사용할 백엔드를 비트 플래그로 지정한다:
    //   kInProcessBackend  — 프로세스 내부에서 트레이스 수집 (데몬 불필요)
    //   kSystemBackend     — 시스템 트레이스 데몬(traced)에 연결 (Linux/Android)
    //
    // 게임 서버는 Windows에서도 돌아가고 별도 데몬 설치가 번거로우므로
    // kInProcessBackend를 사용한다. 이 모드에서는:
    //   - 프로세스 메모리 안에 링 버퍼를 할당하여 이벤트를 기록
    //   - 외부 의존성 없이 독립적으로 동작
    //   - Stop() 후 ReadTrace()로 데이터를 꺼낼 수 있음
    perfetto::TracingInitArgs args;
    args.backends = perfetto::kInProcessBackend;

    perfetto::Tracing::Initialize(args);

    // TrackEvent 데이터소스 등록.
    // Perfetto는 "데이터소스" 개념으로 이벤트 생산자를 관리한다.
    // TrackEvent는 TRACE_EVENT 매크로 계열의 이벤트를 담당하는 내장 데이터소스.
    // Register()를 호출해야 TRACE_EVENT 매크로가 실제로 동작한다.
    perfetto::TrackEvent::Register();

    spdlog::info("[Tracing] Perfetto 초기화 완료 (in-process backend)");
}

// ---------------------------------------------------------------------------
// StartSession
// ---------------------------------------------------------------------------
// TraceConfig: Perfetto 세션의 전체 설정을 담는 protobuf 메시지.
//
// 주요 구조:
//   TraceConfig
//   ├─ buffers[]            — 링 버퍼 설정 (크기, 덮어쓰기 정책)
//   └─ data_sources[]       — 데이터소스별 활성화 설정
//       └─ config
//           ├─ name          — 데이터소스 이름 ("track_event")
//           └─ track_event_config
//               ├─ enabled_categories[] — 활성화할 카테고리
//               └─ disabled_categories[] — 비활성화할 카테고리
// ---------------------------------------------------------------------------

static perfetto::TraceConfig BuildConfig(const SessionConfig& sessionCfg) {
    perfetto::TraceConfig cfg;

    // --- 버퍼 설정 ---
    // Perfetto는 여러 버퍼를 지원하지만, 보통 하나면 충분하다.
    // FillPolicy::RING_BUFFER: 버퍼가 가득 차면 가장 오래된 데이터를 덮어쓴다.
    //   → 항상 "최근 N분"의 데이터를 유지하는 효과
    // 다른 옵션: DISCARD — 버퍼가 차면 새 이벤트를 버림 (초기 구간만 기록)
    auto* bufferCfg = cfg.add_buffers();
    bufferCfg->set_size_kb(sessionCfg.bufferSizeKb);
    bufferCfg->set_fill_policy(
        perfetto::protos::gen::TraceConfig_BufferConfig_FillPolicy_RING_BUFFER
    );

    // --- 데이터소스 설정 ---
    // "track_event"는 TRACE_EVENT 매크로용 내장 데이터소스 이름.
    // 이 이름은 perfetto SDK가 정한 것으로 변경하면 안 된다.
    auto* dsCfg = cfg.add_data_sources()->mutable_config();
    dsCfg->set_name("track_event");

    // track_event_config는 DataSourceConfig의 sub-message이지만,
    // Perfetto SDK의 gen 클래스에서는 lazy 필드로 처리되어
    // mutable 접근자가 없다. 별도 TrackEventConfig 객체를 만들어
    // 직렬화한 뒤 set_track_event_config_raw()로 설정한다.
    perfetto::protos::gen::TrackEventConfig teCfg;

    if (sessionCfg.enabledCategories.empty()) {
        // 빈 목록 → 와일드카드: 모든 비-debug 카테고리 활성화
        // debug 태그 카테고리는 enableDebugCategories가 true일 때만 포함
        teCfg.add_enabled_categories("*");
        if (!sessionCfg.enableDebugCategories) {
            teCfg.add_disabled_categories("debug");
        }
    } else {
        // 명시적 카테고리 목록만 활성화
        for (const auto& cat : sessionCfg.enabledCategories) {
            teCfg.add_enabled_categories(cat);
        }
        if (sessionCfg.enableDebugCategories) {
            teCfg.add_enabled_categories("debug");
        }
    }

    dsCfg->set_track_event_config_raw(teCfg.SerializeAsString());

    return cfg;
}

void StartSession() {
    StartSession(SessionConfig{});
}

void StartSession(const SessionConfig& config) {
    if (g_session) {
        spdlog::warn("[Tracing] 이미 활성 세션이 있습니다. 기존 세션을 먼저 종료하세요.");
        return;
    }

    // NewTrace(kInProcessBackend): in-process 백엔드에서 새 트레이스 세션을 생성.
    // 반환된 TracingSession 객체로 세션의 수명을 관리한다.
    g_session = perfetto::Tracing::NewTrace(perfetto::kInProcessBackend);
    g_session->Setup(BuildConfig(config));

    // Start()를 호출하면 설정된 카테고리의 TRACE_EVENT가 실제로 기록되기 시작한다.
    // 이 시점부터 StopBlocking()까지가 트레이스 구간이다.
    g_session->StartBlocking();

    spdlog::info("[Tracing] 세션 시작 (버퍼: {}KB)", config.bufferSizeKb);
}

// ---------------------------------------------------------------------------
// StopSession
// ---------------------------------------------------------------------------
bool StopSession(const std::filesystem::path& outputPath) {
    if (!g_session) {
        spdlog::warn("[Tracing] 활성 세션이 없습니다.");
        return false;
    }

    // FlushBlocking(): 아직 커밋되지 않은 이벤트를 버퍼에 확정한다.
    // 멀티스레드 환경에서 각 스레드의 TLS(Thread-Local Storage) 버퍼에
    // 남아있는 이벤트를 중앙 버퍼로 옮기는 작업.
    // timeout_ms를 주지 않으면 기본 5초. 서버 종료 시에는 충분히 줘도 된다.
    g_session->FlushBlocking(/* timeout_ms = */ 10000);

    // StopBlocking(): 세션을 종료하고 모든 내부 처리를 완료한다.
    // 이 이후로는 TRACE_EVENT가 기록되지 않는다.
    g_session->StopBlocking();

    // ReadTrace(): 버퍼에 수집된 바이너리 트레이스 데이터를 읽는다.
    // 콜백 방식으로 데이터 청크를 전달하며, 한 번에 모든 데이터를 메모리에
    // 올리지 않아도 스트리밍 방식으로 파일에 쓸 수 있다.
    std::ofstream ofs(outputPath, std::ios::binary);
    if (!ofs) {
        spdlog::error("[Tracing] 파일 열기 실패: {}", outputPath.string());
        g_session.reset();
        return false;
    }

    g_session->ReadTrace([&ofs](perfetto::TracingSession::ReadTraceCallbackArgs args) {
        // args.data: 트레이스 데이터 청크의 포인터
        // args.size: 청크 크기
        // args.has_more: 아직 더 읽을 데이터가 있는지 여부
        ofs.write(args.data, static_cast<std::streamsize>(args.size));
    });

    ofs.close();
    g_session.reset();

    spdlog::info("[Tracing] 트레이스 저장 완료: {}", outputPath.string());
    spdlog::info("[Tracing] https://ui.perfetto.dev 에서 파일을 열어 분석하세요.");
    return true;
}

// ---------------------------------------------------------------------------
// IsSessionActive
// ---------------------------------------------------------------------------
bool IsSessionActive() {
    return g_session != nullptr;
}

}  // namespace psh::tracing
