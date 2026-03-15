# C++ 코딩 컨벤션 (cpp-style.md)

## 언어 버전
C++20

---

## 네이밍

| 대상 | 규칙 | 예시 |
|------|------|------|
| 클래스/구조체 | PascalCase | `CombatSystem`, `DamageResult` |
| 함수/메서드 | camelCase | `calculateDamage()`, `getHealth()` |
| 멤버 변수 | m_ 접두사 + camelCase | `m_health`, `m_sessionId` |
| 정적 멤버 | s_ 접두사 | `s_instance` |
| 상수/enum 값 | UPPER_SNAKE_CASE | `MAX_PARTY_SIZE`, `SKILL_TYPE_FIRE` |
| 로컬 변수 | camelCase | `damageValue`, `targetId` |
| 파일명 | snake_case | `combat_system.h`, `damage_result.h` |

---

## 파일 구조

헤더 파일 순서:
```cpp
// 1. 대응하는 .h (cpp 파일인 경우)
#include "combat_system.h"

// 2. 같은 레이어 헤더
#include "stat_calculator.h"

// 3. 하위 레이어 헤더
#include "config/skill_data.h"
#include "types/stat.h"

// 4. 외부 라이브러리
#include <spdlog/spdlog.h>

// 5. STL
#include <vector>
#include <memory>
#include <optional>
```

---

## 소유권 패턴

```cpp
// ✅ 단독 소유: unique_ptr
std::unique_ptr<CombatSystem> m_combat;

// ✅ 공유 소유 (꼭 필요할 때만): shared_ptr
std::shared_ptr<SessionData> m_session;

// ✅ 비소유 관찰: raw pointer 또는 weak_ptr
// raw pointer: 생명주기가 명확히 보장될 때
const SkillData* m_skillData;   // Config가 소유, 서버 생존 동안 유효
// weak_ptr: shared_ptr 소유, 수명 불확실
std::weak_ptr<PartyMember> m_leader;

// ❌ new/delete 직접 사용 금지
CombatSystem* sys = new CombatSystem(); // 금지
```

---

## 오류 처리

```cpp
// ✅ 실패 가능한 연산: std::optional 또는 Result 패턴
std::optional<DamageResult> calculateHit(EntityId attacker, EntityId target);

// ✅ 설정/초기화 오류: 예외 (서버 시작 시점만)
// ✅ 런타임 게임 로직 오류: optional/error code (예외 사용 금지)
// ❌ 게임 루프 안에서 예외 throw 금지
```

---

## 클래스 설계

```cpp
class CombatSystem {
public:
    // 생성자에서 초기화 완료. 별도 Init() 패턴 지양
    explicit CombatSystem(const SkillConfig& config);

    // 인터페이스는 const 명확히
    DamageResult calculateDamage(const AttackParams& params) const;
    void applyDamage(EntityId target, int damage);

    // 복사 금지, 이동 명시
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;

private:
    const SkillConfig& m_config;  // 참조 — 소유하지 않음
    // ...
};
```

---

## 금지 사항

```cpp
// ❌ using namespace std (헤더에서 절대 금지, cpp에서도 지양)
using namespace std;

// ❌ C 스타일 캐스트
int x = (int)someFloat;  // → static_cast<int>(someFloat)

// ❌ #define 상수
#define MAX_HP 1000  // → constexpr int kMaxHp = 1000;

// ❌ 매직 넘버
if (party.size() > 4) { ... }  // → if (party.size() > kMaxPartySize)

// ❌ 레이어 위반 include (check_layers.py가 감지)
// src/core/combat.h 에서:
#include "network/packet.h"  // Core → Network 금지
```

---

## 로깅

```cpp
// spdlog 사용. 레벨 기준:
spdlog::trace("{}에게 {}데미지", targetId, damage);   // 상세 추적
spdlog::debug("히트 판정: attacker={}", attackerId);  // 디버그
spdlog::info("던전 인스턴스 {} 생성", dungeonId);     // 주요 이벤트
spdlog::warn("패킷 순서 이상: seq={}", seqNum);       // 비정상이나 치명적이지 않음
spdlog::error("세션 {} 처리 실패: {}", sessionId, e.what()); // 오류

// Network, Runtime 레이어에서만 사용 권장
// Core 레이어는 로깅 의존 없이 순수 로직 유지
```

---

## 포맷

clang-format 사용. `.clang-format` 파일 기준.
PR 전 반드시 실행:
```bash
clang-format -i $(find src -name "*.cpp" -o -name "*.h")
```
