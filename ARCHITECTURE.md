# ARCHITECTURE.md
> 프로젝트의 도메인 지도와 레이어 의존성 규칙을 정의합니다.
> 이 규칙은 `tools/check_layers.py`와 CI에서 기계적으로 강제됩니다.

---

## 레이어 의존성 규칙

```
Types → Config → Core → Service → Network → Runtime
```

- 각 레이어는 **자신의 왼쪽에 있는 레이어만** import/include 가능
- 오른쪽 방향 의존은 **CI에서 자동 거부**됨
- 같은 레이어 내 순환 의존 금지

### 위반 예시 (절대 금지)
```cpp
// src/core/combat.h 에서
#include "network/packet.h"  // ❌ Core가 Network를 의존 — 위반
#include "service/party.h"   // ❌ Core가 Service를 의존 — 위반
#include "config/skill_data.h" // ✅ Core가 Config를 의존 — 허용
#include "types/stat.h"        // ✅ Core가 Types를 의존 — 허용
```

---

## 레이어별 책임

### `Types` (`src/types/`)
- POD 구조체, enum, 공용 상수
- 외부 의존 없음 (STL만 허용)
- 예: `EntityId`, `StatType`, `DamageResult`, `ItemId`

### `Config` (`src/config/`)
- 게임 데이터 정의 로더 (JSON → 구조체)
- FlatBuffers 스키마 포함
- `Types`만 의존
- 예: `SkillDataLoader`, `MonsterConfig`, `ItemConfig`

### `Core` (`src/core/`)
- 순수 게임 로직. 네트워크/세션 개념 없음
- `Types`, `Config`만 의존
- 예: `CombatSystem`, `StatCalculator`, `HitDetection`, `InventoryManager`
- **핵심 원칙**: Core는 네트워크를 모른다. 테스트 가능해야 한다.

### `Service` (`src/service/`)
- 여러 Core 시스템을 조합한 게임플레이 흐름
- `Types`, `Config`, `Core`만 의존
- 예: `DungeonInstance`, `PartyManager`, `MatchmakingService`

### `Network` (`src/network/`)
- 패킷 직렬화/역직렬화, 세션 관리, AOI
- 모든 하위 레이어 의존 가능
- 예: `PacketHandler`, `SessionManager`, `AoiGrid`

### `Runtime` (`src/runtime/`)
- 서버 엔트리포인트, 핫리로드, 설정 관리
- 모든 레이어 의존 가능
- 예: `ServerApp`, `HotReloadManager`, `ConfigWatcher`

---

## 도메인별 상태

상세 품질 등급: [docs/quality/grades.md](./docs/quality/grades.md)

| 도메인 | 레이어 | 구현 상태 | 테스트 커버리지 |
|--------|--------|-----------|----------------|
| Types | Types | 🔲 미시작 | — |
| Config | Config | 🔲 미시작 | — |
| Combat | Core | 🔲 미시작 | — |
| HitDetection | Core | 🔲 미시작 | — |
| Inventory | Core | 🔲 미시작 | — |
| DungeonInstance | Service | 🔲 미시작 | — |
| PartyManager | Service | 🔲 미시작 | — |
| Matchmaking | Service | 🔲 미시작 | — |
| PacketHandler | Network | 🔲 미시작 | — |
| SessionManager | Network | 🔲 미시작 | — |
| AoiGrid | Network | 🔲 미시작 | — |
| ServerApp | Runtime | 🔲 미시작 | — |
| HotReload | Runtime | 🔲 미시작 | — |

---

## 외부 라이브러리

| 라이브러리 | 용도 | 레이어 제한 |
|-----------|------|------------|
| spdlog | 로깅 | Runtime, Network |
| FlatBuffers | 직렬화 | Config, Network |
| asio (standalone) | 비동기 네트워크 | Network |
| nlohmann/json | 설정 파싱 | Config |
| GoogleTest | 테스트 | tests/ 전용 |

---

## 디렉토리 구조 전체

```
project-root/
├── AGENTS.md                  # 에이전트 진입점 (이 파일의 목차)
├── ARCHITECTURE.md            # 이 파일
├── CMakeLists.txt
├── src/
│   ├── types/
│   ├── config/
│   ├── core/
│   │   ├── combat/
│   │   ├── inventory/
│   │   └── stat/
│   ├── service/
│   │   ├── dungeon/
│   │   ├── party/
│   │   └── matchmaking/
│   ├── network/
│   │   ├── packet/
│   │   ├── session/
│   │   └── aoi/
│   └── runtime/
├── tests/
│   ├── unit/
│   └── integration/
├── data/
│   ├── skills/
│   ├── monsters/
│   └── items/
├── tools/
│   ├── check_layers.py        # 레이어 의존성 검사
│   ├── doc_check.py           # 문서 유효성 검사
│   └── codegen/               # 코드 생성 툴
└── docs/
    ├── design/
    ├── architecture/
    ├── conventions/
    ├── workflow/
    ├── quality/
    └── exec-plans/
        ├── active/
        └── completed/
```
