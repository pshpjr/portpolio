#pragma once

// =============================================================================
// Perfetto Tracing 래퍼
// =============================================================================
//
// Perfetto란?
//   Google이 만든 오픈소스 시스템/앱 성능 추적(tracing) 라이브러리.
//   함수 실행 시간, 커스텀 이벤트, 카운터 등을 기록하여
//   Chrome의 chrome://tracing 또는 https://ui.perfetto.dev 에서 시각화할 수 있다.
//
// 동작 원리:
//   1) 카테고리(Category) 정의 — 추적 이벤트를 논리적 그룹으로 분류
//   2) 초기화(Initialize) — Perfetto 백엔드 등록
//   3) 세션(Session) 시작 — 어떤 카테고리를 기록할지 설정 후 추적 시작
//   4) TRACE_EVENT 매크로 사용 — 코드 곳곳에 삽입하여 성능 데이터 수집
//   5) 세션 종료 → .perfetto-trace 파일로 저장 → UI에서 분석
//
// 사용 예시:
//   // 서버 시작 시 한 번
//   psh::tracing::Init();
//   psh::tracing::StartSession();
//
//   // 측정하고 싶은 코드 영역
//   void ProcessPacket(Packet& pkt) {
//       TRACE_EVENT("network", "ProcessPacket");   // 이 스코프의 실행시간 기록
//       ...
//   }
//
//   // 서버 종료 시
//   psh::tracing::StopSession("server_trace.perfetto-trace");
//
// 결과 파일 열기:
//   https://ui.perfetto.dev 에 .perfetto-trace 파일을 드래그 앤 드롭
// =============================================================================

#include <perfetto.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

// =============================================================================
// 트레이스 카테고리 정의
// =============================================================================
// Perfetto는 카테고리별로 이벤트를 ON/OFF 할 수 있다.
// 예를 들어, "network" 카테고리만 켜면 네트워크 관련 이벤트만 기록된다.
//
// 카테고리 정의는 반드시 헤더에서 PERFETTO_DEFINE_CATEGORIES로 선언하고,
// 정확히 하나의 .cpp 파일에서 PERFETTO_TRACK_EVENT_STATIC_STORAGE를 호출해야 한다.
//
// 사용 가능한 카테고리:
//   "network"  — 패킷 송수신, 연결 관리 등 네트워크 I/O
//   "game"     — 게임 로직 틱, 상태 업데이트, 월드 시뮬레이션
//   "db"       — 데이터베이스 쿼리, 트랜잭션
//   "config"   — 서버 설정 로딩, 핫 리로드
//   "item"     — 인벤토리 조작, 아이템 생성/소멸
//   "user"     — 유저 세션 관리, 인증
//   "debug"    — 디버깅 전용 (기본 OFF, 필요 시 활성화)
// =============================================================================
PERFETTO_DEFINE_CATEGORIES(
    // perfetto::Category("카테고리명")
    //   .SetDescription("설명") — UI에서 카테고리 위에 마우스를 올리면 표시됨
    //   .SetTags("slow")       — "slow" 태그가 붙은 카테고리는 기본적으로 비활성화됨

    perfetto::Category("network").SetDescription("네트워크 I/O: 패킷 송수신, 연결/해제, 세션 관리"),

    perfetto::Category("game").SetDescription("게임 로직: 틱 업데이트, 전투, 상태 전이"),

    perfetto::Category("db").SetDescription("데이터베이스: 쿼리 실행, 커넥션 풀"),

    perfetto::Category("config").SetDescription("설정: JSON 로딩, 환경변수 오버라이드"),

    perfetto::Category("item").SetDescription("아이템: 인벤토리 조작, 장비 장착/해제"),

    perfetto::Category("user").SetDescription("유저: 로그인/로그아웃, 세션 라이프사이클"),

    // debug 카테고리는 기본 비활성화.
    // StartSession에서 명시적으로 켜야만 기록된다.
    perfetto::Category("debug").SetDescription("디버깅 전용 상세 트레이스").SetTags("debug"));

namespace psh::tracing
{

// =============================================================================
// 초기화 / 세션 관리
// =============================================================================

// Perfetto 백엔드를 등록한다. 서버 시작 시 한 번만 호출.
//
// 내부적으로 perfetto::Tracing::Initialize()를 호출하여
// in-process 백엔드(TracingBackend)를 등록한다.
// in-process 모드는 별도 데몬 없이 프로세스 내부에서 트레이스 데이터를 수집한다.
void Init();

// ---------------------------------------------------------------------------
// 세션 설정
// ---------------------------------------------------------------------------
// TraceConfig로 어떤 카테고리를 기록할지, 버퍼 크기 등을 조정할 수 있다.
// StartSession()은 기본 설정을 사용하고,
// StartSessionWithConfig()는 사용자 지정 설정을 받는다.

struct SessionConfig
{
    // 트레이스 데이터를 저장할 링 버퍼 크기 (바이트 단위).
    // 버퍼가 가득 차면 가장 오래된 데이터부터 덮어쓴다.
    // 기본값 64MB — 수분~수십분 분량의 트레이스를 저장할 수 있다.
    // 장시간 프로파일링이 필요하면 늘리고, 메모리가 부족하면 줄인다.
    uint32_t bufferSizeKb = 64 * 1024;

    // 기록할 카테고리 이름 목록.
    // 비어 있으면 "debug" 태그가 붙지 않은 모든 카테고리를 기록한다.
    // 예: {"network", "game"} → 네트워크와 게임 로직만 기록
    std::vector<std::string> enabledCategories;

    // true면 "debug" 태그가 붙은 카테고리도 활성화한다.
    // 디버그 카테고리는 매우 상세한 이벤트를 남기므로
    // 오버헤드가 클 수 있어 기본적으로 꺼둔다.
    bool enableDebugCategories = false;
};

// 기본 설정으로 트레이스 세션을 시작한다.
// (64MB 버퍼, debug 제외 전체 카테고리 활성화)
void StartSession();

// 사용자 지정 설정으로 트레이스 세션을 시작한다.
void StartSession(const SessionConfig& config);

// 현재 세션을 중지하고 트레이스 데이터를 파일로 저장한다.
// 반환: 저장 성공 시 true, 활성 세션이 없으면 false.
//
// 저장된 .perfetto-trace 파일은 https://ui.perfetto.dev 에서 열 수 있다.
bool StopSession(const std::filesystem::path& outputPath);

// 현재 트레이스 세션이 활성 상태인지 확인한다.
bool IsSessionActive();

// =============================================================================
// 편의 매크로
// =============================================================================
// Perfetto의 TRACE_EVENT 매크로를 직접 사용해도 되지만,
// 프로젝트 네이밍 컨벤션에 맞춘 별칭을 제공한다.
//
// 기본 사용법:
//   PSH_TRACE_EVENT("category", "EventName");
//   → 현재 스코프(함수/블록)의 시작~끝 시간을 자동으로 기록
//
// 인자 포함:
//   PSH_TRACE_EVENT("network", "SendPacket", "size", packetSize);
//   → 이벤트에 "size"라는 이름으로 packetSize 값을 첨부
//
// 즉시 이벤트 (duration이 아닌 시점 기록):
//   PSH_TRACE_EVENT_INSTANT("game", "PlayerDied");
//   → 특정 순간만 마킹 (스코프 추적 없음)
//
// 비동기 이벤트 (스코프에 묶이지 않는 시작/끝):
//   PSH_TRACE_EVENT_BEGIN("db", "LongQuery");
//   ... 비동기 작업 ...
//   PSH_TRACE_EVENT_END("db");
//   → 시작과 끝이 다른 스코프에 있을 때 사용

// 스코프 기반 트레이스 이벤트 (가장 자주 사용)
// 블록이 끝나면 자동으로 duration이 기록된다.
#define PSH_TRACE_EVENT(category, name, ...) TRACE_EVENT(category, name, ##__VA_ARGS__)

// 즉시(instant) 이벤트 — 시점만 기록, duration 없음
#define PSH_TRACE_EVENT_INSTANT(category, name, ...) \
    TRACE_EVENT_INSTANT(category, name, ##__VA_ARGS__)

// 비동기 시작/끝 — 서로 다른 스코프에서 짝을 맞춰 사용
#define PSH_TRACE_EVENT_BEGIN(category, name, ...) TRACE_EVENT_BEGIN(category, name, ##__VA_ARGS__)

#define PSH_TRACE_EVENT_END(category, ...) TRACE_EVENT_END(category, ##__VA_ARGS__)

// 카운터 트레이스 — 시간에 따른 숫자 값 변화를 그래프로 표시
// 예: PSH_TRACE_COUNTER("network", "ActiveConnections", connectionCount);
//     → UI에서 시간축 위에 선 그래프로 표시됨
#define PSH_TRACE_COUNTER(category, name, value) TRACE_COUNTER(category, name, value)

} // namespace psh::tracing
