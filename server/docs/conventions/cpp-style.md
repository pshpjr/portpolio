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
| 클래스/구조체 | PascalCase | `CombatSystem`, `DamageResult` |
| 함수/메서드 | PascalCase | `CalculateDamage()`, `GetHealth()` |
| 멤버 변수 | `m_` 접두사 + camelCase | `m_health`, `m_sessionId` |
| 정적 멤버 | `s_` 접두사 | `s_instance` |
| 상수/enum 값 | UPPER_SNAKE_CASE | `MAX_PARTY_SIZE`, `SKILL_TYPE_FIRE` |
| 로컬 변수 | camelCase | `damageValue`, `targetId` |
| 파일명 | snake_case | `combat_system.h`, `damage_result.h` |

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
std::unique_ptr<CombatSystem> m_combat;

// 공유 소유는 꼭 필요할 때만
std::shared_ptr<SessionData> m_session;

// 비소유 참조
const SkillData* m_skillData;
std::weak_ptr<PartyMember> m_leader;

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
class CombatSystem {
public:
    explicit CombatSystem(const SkillConfig& config);

    DamageResult CalculateDamage(const AttackParams& params) const;
    void ApplyDamage(EntityId target, int damage);

    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;

private:
    const SkillConfig& m_config;
};
```

- 생성자에서 유효한 상태를 만든다.
- `Init()` 같은 이단계 초기화 패턴은 특별한 이유가 없으면 도입하지 않는다.
- 복사/이동 가능성은 의도적으로 선언한다.

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
