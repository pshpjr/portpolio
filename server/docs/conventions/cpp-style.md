# C++ 코딩 컨벤션

## 적용 범위

- `server/`와 `Lib/`의 공용 C++ 규칙이다.
- 서버 레이어 규칙은 [../../ARCHITECTURE.md](../../ARCHITECTURE.md)를 따른다.
- 앱 전용 예외가 필요하면 각 앱 문서에서 추가 규칙만 덧붙인다.

---

## 언어 버전

C++23

---

## 네이밍

| 대상 | 규칙 | 예시 |
|------|------|------|
| 클래스/구조체 | PascalCase | `Histogram`, `JobQueue` |
| 함수/메서드 | PascalCase | `Record()`, `GetStatsSnapshot()` |
| private 멤버 변수 | camelCase + 접미사 `_` | `upperBounds_`, `executor_` |
| public 데이터 멤버 (POD/snapshot 구조체) | PascalCase, 접두/접미사 없음 | `LowerBound`, `TotalCount` |
| 정적 멤버 | 동일 규칙 + `static` | `nextId_` |
| 상수/enum 값 | UPPER_SNAKE_CASE 또는 PascalCase enum class | `EnumJobState::Queued`, `MAX_PARTY_SIZE` |
| 로컬 변수 | camelCase | `targetCount`, `cumulativeCount` |
| 네임스페이스 | snake_case | `psh::lib::telemetry` |
| 파일명 | snake_case | `histogram.h`, `job_queue.h` |

- 헤더 외부에 노출되는 데이터(스냅샷·옵션 구조체 필드)는 PascalCase. 외부 호출자가 공개 API 필드와 헷갈리지 않도록 멤버 접미사 `_` 를 쓰지 않는다.
- 클래스 내부 상태(behavior 가 붙는 멤버)는 항상 `xxx_` 접미사로 통일한다. `m_` 접두사는 사용하지 않는다.

---

## 파일 구조

헤더 include 순서:

```cpp
// 1. 대응하는 .h (cpp 파일인 경우)
#include "combat_system.h"

// 2. 같은 계층 헤더
#include "stat_calculator.h"

// 3. 더 기초적인 계층 헤더
#include "config/skill_data.h"
#include "types/stat.h"

// 4. 외부 라이브러리
#include <spdlog/spdlog.h>

// 5. STL
#include <memory>
#include <optional>
#include <vector>
```

---

## 소유권 패턴

```cpp
// 단독 소유
std::unique_ptr<CombatSystem> combat_;

// 공유 소유는 꼭 필요할 때만
std::shared_ptr<SessionData> session_;

// 비소유 참조
const SkillData* skillData_;
std::weak_ptr<PartyMember> leader_;

// 직접 new/delete 금지
CombatSystem* sys = new CombatSystem(); // 금지
```

- null이 될 수 없고 수명이 명확하면 reference를 우선한다.
- shared ownership가 필요하지 않으면 `shared_ptr`를 도입하지 않는다.

---

## 오류 처리

```cpp
// 실패 가능한 연산 — std::expected를 기본으로 사용
std::expected<DamageResult, ErrorCode> CalculateHit(EntityId attacker, EntityId target);

// 오류 정보가 필요 없으면 std::optional도 허용
std::optional<EntityId> FindTarget(Position pos);
```

- **예외(exception)는 사용하지 않는다.** 외부 라이브러리(Boost, spdlog 등)가 던지는 예외를 받아야 하는 경계에서만 `try/catch`를 허용한다.
- 실패 가능한 연산은 `std::expected<T, E>`를 기본 반환 타입으로 사용한다.
- 오류 종류 구분이 필요 없는 단순 실패는 `std::optional<T>`을 허용한다.
- 복구 불가능한 프로그래밍 오류(불변식 위반)는 `assert` 또는 즉시 종료로 처리한다.

---

## 클래스 설계

```cpp
class Histogram
{
public:
    explicit Histogram(std::vector<uint64_t> upperBounds);

    void Record(uint64_t value) noexcept;
    [[nodiscard]] HistogramSnapshot Snapshot() const;

    Histogram(const Histogram&) = delete;
    Histogram& operator=(const Histogram&) = delete;

private:
    std::vector<uint64_t> upperBounds_;
    std::vector<std::atomic<uint64_t>> counts_;
    std::atomic<uint64_t> totalCount_{0};
};
```

- 생성자에서 유효한 상태를 만든다. `Init()` 같은 이단계 초기화는 도입하지 않는다.
- 복사/이동 가능성은 의도적으로 선언한다.
- Allman brace. `public:` / `private:` 는 들여쓰기 없이 클래스 본문 첫 컬럼에 둔다.
- 읽기 전용 반환에는 `[[nodiscard]]` 를 붙인다.

### 스냅샷·옵션 구조체

```cpp
struct HistogramSnapshot
{
    std::vector<HistogramBucket> Buckets;
    uint64_t TotalCount = 0;
    uint64_t Sum        = 0;
    uint64_t Maximum    = 0;
    bool IsApproximate  = false;
};
```

- 모든 필드에 **기본값 in-class 초기화** 를 단다. 호출측에서 designated initializer 로 일부만 채워도 안전하도록 보장한다.
- 같은 블록 안의 `=` 위치를 **공백으로 정렬** 한다 (clang-format `AlignConsecutiveAssignments`).
- 주석은 단위·의미만 짧게 (`// inclusive`, `// ns`).

### 인스턴스 생성

```cpp
snap.Buckets.push_back({
    .LowerBound = lower,
    .UpperBound = upperBounds_[i],
    .Count      = counts_[i].load(std::memory_order_relaxed),
});
```

- POD/스냅샷 구조체 생성은 **designated initializer** 를 우선한다. 위치 기반 생성자보다 가독성이 우선.

---

## 표준 라이브러리 활용

### Ranges 우선

```cpp
// 권장
const size_t idx = std::ranges::lower_bound(upperBounds_, value) - upperBounds_.begin();

// 지양 (iterator pair 가 꼭 필요한 경우만)
auto it = std::lower_bound(upperBounds_.begin(), upperBounds_.end(), value);
```

- 반복자 쌍을 두 번 적는 대신 `std::ranges::*` 알고리즘을 우선한다. 부분 범위가 정말 필요할 때만 iterator-pair 형태를 쓴다.

### 동시성 헬퍼 재사용

```cpp
// 권장 (psh::lib::utils 의 공용 헬퍼 사용)
utils::FetchMax(max_, value, std::memory_order_relaxed);

// 지양 (CAS 루프 인라인 반복 작성)
uint64_t prev = max_.load(std::memory_order_relaxed);
while (value > prev && !max_.compare_exchange_weak(prev, value, ...)) {}
```

- atomic CAS 패턴(`FetchMax` / `FetchMin` / `FetchOr` 등)은 `psh::lib::utils` 의 헬퍼로 모은다. 같은 패턴을 두 번 손으로 적기 전에 헬퍼를 추가하거나 기존 헬퍼를 찾는다.

### 구현 디테일은 `namespace detail`

```cpp
namespace detail
{
inline uint64_t EstimatePercentile(const HistogramSnapshot& snap, double q) noexcept;
}

inline DistributionSummary Summarize(const HistogramSnapshot& snap) noexcept;
```

- 외부에 노출하지 않을 보조 함수는 anonymous namespace 가 아니라 같은 헤더 안의 `detail` 네임스페이스에 둔다 (헤더 온리 라이브러리 기준).

---

## 함수 본문 스타일

### 함수 내부 `using namespace`

```cpp
[[nodiscard]] RateSnapshot Snapshot()
{
    using namespace std::chrono;

    const auto now = steady_clock::now();
    if (auto elapsedNs = duration_cast<nanoseconds>(now - lastSampleTime_).count();
        elapsedNs > 0)
    {
        // ...
    }
}
```

- 헤더/네임스페이스 스코프 `using namespace std;` 는 금지. 단, **함수 본문 안의 좁은 스코프에서 `std::chrono` 같은 자주 쓰는 서브 네임스페이스를 풀어 쓰는 것은 허용**한다 (`duration_cast<std::chrono::nanoseconds>` 같은 긴 prefix 를 줄이기 위함).
- 풀어 쓰는 범위는 **함수 본문 한정**. 같은 헤더의 다른 함수에 영향을 주지 않는다.

### `if (init; cond)` 로 변수 스코프 좁히기

```cpp
// 권장 — elapsedNs 가 if 블록 안에서만 살아있음
if (auto elapsedNs = duration_cast<nanoseconds>(now - lastSampleTime_).count();
    elapsedNs > 0)
{
    snap.PerSecond = ...;
}

// 지양 — 함수 끝까지 elapsedNs 가 살아있음
const auto elapsedNs = duration_cast<nanoseconds>(now - lastSampleTime_).count();
if (elapsedNs > 0)
{
    snap.PerSecond = ...;
}
```

- 조건문에서만 쓰는 임시 값은 C++17 init-statement 로 if/switch 안에 가둔다.

### 지연 멤버 호출은 `[this, self = shared_from_this()]`

```cpp
// 권장 — this 캡처로 호출 편의, self 캡처는 순수 수명 anchor
class JobQueue : public std::enable_shared_from_this<JobQueue>
{
    void DrainOnce()
    {
        if (executor_->Post([this, self = shared_from_this()]
                            {
                                DrainOnce();
                            }))
        { ... }
    }
};

// 지양 — self-> prefix 가 본문을 무겁게 만들고, this 미캡처라 멤버 접근이 어색
auto self = shared_from_this();
executor_->Post([self] { self->DrainOnce(); });
```

- `enable_shared_from_this` 타입에서 비동기/지연 호출에 멤버를 넘길 때는 `[this, self = shared_from_this()]` 묶음을 쓴다.
  - `this` — 람다 본문에서 `member_()` / `Method()` 를 자연스럽게 호출하기 위함.
  - `self` — `*this` 가 람다 실행 시점까지 살아있도록 하는 **수명 anchor**. 본문에서 직접 참조하지 않아도 캡처만으로 충분.
- 본문에서 `self->Foo()` 처럼 self 를 직접 쓰지 않는다 — anchor 와 호출자를 의도적으로 분리해서 표현한다.

```cpp
// 권장 — 외부 관측 의미는 read-only 이므로 const, 내부 샘플 캐시·동기화 멤버는 mutable
class RateMeter
{
public:
    [[nodiscard]] RateSnapshot Snapshot() const;

private:
    std::atomic<uint64_t> total_{0};

    mutable std::mutex sampleMtx_;
    mutable uint64_t lastSampledTotal_ = 0;
    mutable std::chrono::steady_clock::time_point lastSampleTime_;
};
```

- 호출자 입장에서 **논리적으로 read-only** (관측만 함) 이면 메서드는 `const` 로 둔다. 내부 캐시·샘플·동기화 멤버는 `mutable` 로 풀어준다.
- 단순히 const 를 피하기 위해 메서드를 non-const 로 두지 않는다. 호출측에서 `const RateMeter&` 로도 관측 가능해야 한다.

---

## 불필요한 캐스팅 지양

```cpp
// 권장 — int64_t (count()) → uint64_t (Record 매개변수) 는 implicit 으로 충분
waitHistogram_.Record(duration_cast<nanoseconds>(startedAt - entry->ScheduledAt).count());

// 지양 — 의미 없는 명시적 cast 가 본문을 무겁게 만든다
waitHistogram_.Record(
    static_cast<uint64_t>(duration_cast<nanoseconds>(startedAt - entry->ScheduledAt).count()));
```

```cpp
// 권장 — 정밀도 보존을 위한 cast 는 유지 (int → double 강제)
snap.PerSecond = static_cast<double>(snap.Total - lastSampledTotal_) * 1e9 / elapsedNs;
```

- 같은 너비의 signed → unsigned, `size_t` ↔ `uint64_t` 같은 동의어 변환은 implicit 에 맡긴다.
- cast 는 **의미가 바뀌는 자리** 에만 쓴다: 정밀도 변환(`int → double`), 좁은 타입으로 narrowing, 불완전 타입 ↔ 포인터 등.
- C 스타일 캐스트(`(uint64_t)x`) 는 어떤 경우에도 쓰지 않는다.

---

## 기본 초기화는 `{}`

```cpp
// 권장 — value-init 가 0/nullptr/기본 생성을 알아서 처리
std::atomic<uint64_t> totalCount_{};
std::atomic<EntryId> nextId_{};

// 지양 — 타입에 종속적인 0 리터럴을 한 번 더 적는 잡음
std::atomic<uint64_t> totalCount_{0};
std::atomic<EntryId> nextId_{0};
```

- 멤버 초기화에 영(zero) 또는 기본 생성을 원할 때는 `{}` 만 적는다. 산술 타입은 `0`, 포인터는 `nullptr`, 클래스 타입은 기본 생성자가 호출된다.
- 의미 있는 초깃값(0 이 아닌 상수, 센티넬 등)은 그 값을 적는다 — `{0}` 는 의미 있는 0 이 아니라 잡음일 때만 정리 대상이다.

---

## 기본값 반환은 `return {};`

```cpp
// 권장 — 반환 타입은 시그니처가 이미 말해주고 있음
JobHandle Post(Callback fn, std::string_view debugLabel = {})
{
    if (!fn)
        return {};
    // ...
}

// 지양 — 타입을 한 번 더 적는 잡음
return JobHandle{};
```

- 함수가 기본 생성된 반환 타입을 돌려줄 때는 `return {};` 만 적는다. 타입명은 시그니처가 단일 소스다.
- `std::optional<T>` 의 nullopt 도 `return {};` 로 통일한다 (`return std::nullopt;` 보다 짧다).
- 단, 동일 타입이 여러 후보(예: `std::variant`)거나 brace-init 가 모호한 자리에서는 명시적 타입을 유지한다.

---

## 방어 코드 최소화

```cpp
// 권장 — sentinel(UINT64_MAX) 가 보장하므로 idx 는 항상 유효
const size_t idx = std::ranges::lower_bound(upperBounds_, value) - upperBounds_.begin();
counts_[idx].fetch_add(1, ...);

// 지양 — 이미 보장된 불변에 대한 중복 가드
if (idx >= upperBounds_.size())
    idx = upperBounds_.size() - 1;
```

- 클래스 내부에서 보장한 불변(생성자가 sentinel 삽입, 멤버가 not-null 등)에 대해 같은 함수 안에서 다시 가드하지 않는다. 보장 위치를 한 곳으로 모으고, 호출지점은 신뢰한다.
- 외부 입력 경계(생성자 인자, 공개 API 인자) 에서만 검증한다.

---

## 금지 사항

```cpp
using namespace std;        // 헤더에서 금지, cpp에서도 지양
int x = (int)someFloat;     // C 스타일 캐스트 금지
#define MAX_HP 1000         // 상수 매크로 금지
if (party.size() > 4) { }   // 매직 넘버 지양
```

- 프로젝트 계층 규칙을 깨는 include를 금지한다.
- 의존관계 위반은 `tools/check_layers.py`로 검사한다.

---

## 로깅

- 로깅 백엔드 선택은 앱 계층 책임이다.
- `Lib/`와 순수 규칙 중심 모듈은 로깅 의존 없이 재사용 가능해야 한다.
- 앱 계층에서 로깅이 필요할 때는 로그 레벨 기준을 명확히 유지한다.

---

## 포맷

- `.clang-format`을 기준으로 `clang-format`을 적용한다.
- 포맷 명령은 작업 영역 문서에 정의된 절차를 따른다.
