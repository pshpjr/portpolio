# 인카운터 데이터 테이블 구조

## 목적

보스 전투 프로필, 패턴, 전조, 피드백, 체크포인트, 기믹 authoring에 필요한 테이블 구조와 책임 범위를 정의한다.
실제 행 데이터는 별도 JSON/CSV로 관리하며, 이 문서는 컬럼 계약과 테이블 간 연결 규칙만 다룬다.

---

## 테이블 전체 관계 다이어그램

```
BossCombatProfileTable.BossId
  → BossPatternTable.BossId            (패턴 행 조회)
  → BossStaggerRuleTable.BossId        (무력화 단계 규칙 조회)

BossPatternTable.PatternId
  → BossTelegraphTable.TelegraphId     (전조 연출 조회)

CombatFeedbackTable.FeedbackTag
  ← 서버 이벤트 (PARRY_SUCCESS 등)    (피드백 표시 결정)

DungeonCheckpointTable.CheckpointId
  ← DungeonId + AreaId                 (구역 진입 시 체크포인트 확인)

DungeonGimmickTable.GimmickId
  ← DungeonId + AreaId                 (기믹 발동 조건 조회)
```

---

## 테이블 정의

### `BossCombatProfileTable` — 보스 공통 전투 프로필

> BossCombatProfileTable은 보스의 최대 HP, 방어력, 무력화 게이지, 상태이상 저항, 광폭화 시간을 저장하며, 서버가 보스 인스턴스 생성 시 단일 조회로 전투 수치 기반을 설정할 수 있게 한다.

*(pending decision #8 기본안 A 반영 — 보스 전투 프로필을 패턴 테이블과 분리)*

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `BossId` | INT | PK | |
| `BossCode` | VARCHAR | DATA | 내부 코드명 (예: `SPIRIT_SHAMAN`) |
| `BossName` | VARCHAR | DATA | 표시 이름 |
| `MaxHP` | INT | DATA | 최대 HP |
| `Defense` | FLOAT | DATA | 방어율 |
| `StaggerGaugeMax` | INT | DATA | 무력화 게이지 최대값 |
| `StaggerRecoverySec` | FLOAT | DATA | 무력화 이후 회복 시간 (초) |
| `StatusResistanceTag` | VARCHAR | DATA | 상태이상 저항 태그 (쉼표 구분, 예: `STUN,SLOW`) |
| `EnrageTimeSec` | FLOAT | DATA | 광폭화 발동까지 남은 시간 (0 = 없음) |
| `EnrageHpThreshold` | FLOAT | DATA | HP 비율 기반 광폭화 임계값 (0.0 = 사용 안 함) |
| `DownProtectionSec` | FLOAT | DATA | 다운 후 재무력화 보호 시간 (초) |
| `PhaseCount` | INT | DATA | 총 페이즈 수 (혼식 주술사 = 2) |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `BossId`
- Unique: `BossCode + BalanceVersion`

---

### `BossStaggerRuleTable` — 무력화 단계 규칙

> BossStaggerRuleTable은 보스의 무력화 회차별 게이지 임계값, 딜타임 길이, 회차 초기화 조건을 저장하며, 서버가 무력화 진행 시 현재 회차로 딜타임과 다음 임계값을 결정할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `RuleId` | INT | PK | |
| `BossId` | INT | FK | → `BossCombatProfileTable.BossId` |
| `StaggerRound` | INT | DATA | 무력화 회차 (1부터 시작) |
| `GaugeThreshold` | INT | DATA | 해당 회차 무력화 발동 임계값 |
| `DilTimeSec` | FLOAT | DATA | 딜타임 길이 (초) |
| `ResetOnPhase` | BOOL | DATA | 페이즈 전환 시 게이지 초기화 여부 |
| `BonusDropTag` | VARCHAR | DATA | 무력화 성공 시 추가 드롭 태그 (NULL = 없음) |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `RuleId`
- FK: `BossId → BossCombatProfileTable.BossId`
- Unique: `BossId + StaggerRound + BalanceVersion`

---

### `BossPatternTable` — 보스 패턴 정의

> BossPatternTable은 보스의 개별 행동 패턴(전조 ID, 대응 수단, 인터럽트 여부, 실패 페널티)을 저장하며, 서버가 보스 AI 상태 전이 시 현재 패턴에 맞는 판정 규칙을 단일 조회로 가져올 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `PatternId` | INT | PK | |
| `BossId` | INT | FK | → `BossCombatProfileTable.BossId` |
| `PatternCode` | VARCHAR | DATA | 내부 코드명 |
| `DisplayName` | VARCHAR | DATA | 표시 이름 |
| `PhaseTag` | ENUM | DATA | `CORE`, `EXTENSION` |
| `TelegraphId` | INT | FK | → `BossTelegraphTable.TelegraphId` |
| `AnswerTag` | ENUM | DATA | `PARRY`, `DODGE`, `INTERRUPT`, `SPREAD`, `FOCUS_BREAK` |
| `CooldownSec` | FLOAT | DATA | 패턴 재사용 쿨다운 (초) |
| `StaggerDamage` | INT | DATA | 무력화 게이지 기여량 |
| `Interruptible` | BOOL | DATA | 인터럽트 가능 여부 |
| `FailurePenaltyTag` | VARCHAR | DATA | 대응 실패 시 효과 태그 |
| `RewardWindowSec` | FLOAT | DATA | 대응 성공 후 딜타임 길이 (초) |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `PatternId`
- FK: `BossId → BossCombatProfileTable.BossId`
- FK: `TelegraphId → BossTelegraphTable.TelegraphId`

---

### `BossTelegraphTable` — 보스 전조 연출

> BossTelegraphTable은 보스 패턴 발동 전 플레이어에게 보여줄 시각/청각 전조 연출 파라미터를 저장하며, 클라이언트가 패턴 전조 연출을 단일 키로 로드할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `TelegraphId` | INT | PK | |
| `AnimationCue` | VARCHAR | META | 애니메이션 전조 키 |
| `GroundIndicatorShape` | ENUM | META | 장판 형태 (`CIRCLE`, `SECTOR`, `RECTANGLE`, `LINE`, `NONE`) |
| `GroundIndicatorSec` | FLOAT | DATA | 장판 예고 시간 (초) |
| `CastBarSec` | FLOAT | DATA | 시전 바 길이 (초, 0 = 없음) |
| `SoundCue` | VARCHAR | META | 오디오 경고 키 |
| `ScreenWarningTag` | ENUM | META | 화면 경고 형태 (`TOP_BANNER`, `EDGE_FLASH`, `NONE`) |
| `WarningTextKey` | VARCHAR | META | 화면 경고 문구 리소스 키 |
| `CameraPullBackMeters` | FLOAT | DATA | 카메라 후퇴량 (0 = 없음) |
| `SeverityTag` | ENUM | META | 경고 심각도 (`LOW`, `MED`, `HIGH`, `FATAL`) |

- PK: `TelegraphId`

---

### `CombatFeedbackTable` — 전투 피드백 표현

> CombatFeedbackTable은 패링 성공/실패, 무력화 돌입, 광폭화 경고 같은 전투 이벤트에 대한 화면 표시 파라미터를 저장하며, 클라이언트가 이벤트 태그 하나로 피드백 연출 전체를 로드할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `FeedbackTag` | VARCHAR | PK | |
| `TriggerEvent` | ENUM | DATA | `PARRY_SUCCESS`, `PARRY_FAIL`, `INTERRUPT_SUCCESS`, `STAGGER_ENTER`, `ENRAGE_WARNING` |
| `ScreenPosition` | ENUM | META | 표시 위치 (`CENTER`, `TOP`, `BOSS_BAR`) |
| `DurationMs` | INT | DATA | 지속 시간 (ms) |
| `TextKey` | VARCHAR | META | 표시 문구 리소스 키 |
| `SoundCue` | VARCHAR | META | 오디오 키 |
| `Priority` | INT | DATA | 동시 발생 시 우선순위 |
| `StackRule` | ENUM | DATA | `REPLACE`, `QUEUE`, `IGNORE` |
| `SeverityTag` | ENUM | META | 표시 강도 (`LOW`, `MED`, `HIGH`) |

- PK: `FeedbackTag`

---

### `DungeonCheckpointTable` — 던전 체크포인트

> DungeonCheckpointTable은 던전 구역별 체크포인트 저장 조건과 복귀 시 리셋 규칙을 저장하며, 서버가 구역 진입 이벤트에서 부활 앵커와 진행 상태를 단일 행으로 결정할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `CheckpointId` | INT | PK | |
| `DungeonId` | INT | DATA | 던전 ID |
| `AreaId` | INT | DATA | 구역 ID |
| `TriggerCondition` | VARCHAR | DATA | 체크포인트 저장 조건 태그 |
| `RespawnAnchor` | VARCHAR | META | 부활 위치 키 |
| `ResetRule` | ENUM | DATA | 복귀 시 유지/초기화 규칙 (`KEEP_PROGRESS`, `RESET_MOBS`) |

- PK: `CheckpointId`
- Unique: `DungeonId + AreaId`

---

### `DungeonGimmickTable` — 던전 기믹 정의

> DungeonGimmickTable은 던전 구역별 협동 기믹의 성공 조건, 시간 제한, 실패 복구 규칙, 역할 안내 태그를 저장하며, 서버가 기믹 발동 시 규칙 전체를 단일 행으로 읽을 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `GimmickId` | INT | PK | |
| `DungeonId` | INT | DATA | 던전 ID |
| `AreaId` | INT | DATA | 구역 ID |
| `GimmickType` | ENUM | DATA | `ALTAR_SYNC`, `ESCORT` |
| `PartySize` | INT | DATA | 기준 파티 인원 |
| `RequiredCount` | INT | DATA | 성공 필요 수 |
| `TimeLimitSec` | FLOAT | DATA | 시간 제한 (초) |
| `FailureRecoverySec` | FLOAT | DATA | 실패 후 복구 구간 길이 (초) |
| `RetryRule` | ENUM | DATA | 재시도 규칙 (`IMMEDIATE`, `DELAY`, `ONCE`) |
| `RoleHintTag` | VARCHAR | DATA | 역할 안내 태그 (쉼표 구분) |
| `RoleHintTextKey` | VARCHAR | META | 역할 안내 문구 리소스 키 |
| `MarkerKey` | VARCHAR | META | 월드 마커 리소스 키 |
| `FailurePenaltyTag` | VARCHAR | DATA | 기믹 실패 시 페널티 태그 |

- PK: `GimmickId`

---

## 테이블 연결 규칙

```
보스 인스턴스 생성
  → BossCombatProfileTable.BossId      (최대HP, 게이지, 저항, 광폭화 시간)
    → BossStaggerRuleTable.BossId      (무력화 회차별 임계값과 딜타임)
    → BossPatternTable.BossId          (패턴 행 전체 로드)
      → BossTelegraphTable.TelegraphId (패턴별 전조 연출 파라미터)

전투 이벤트 발생 (서버 → 클라)
  → CombatFeedbackTable.TriggerEvent   (화면 위치, 문구 키, 오디오, 우선순위)

던전 구역 진입
  → DungeonCheckpointTable (DungeonId + AreaId)  (부활 앵커, 리셋 규칙)
  → DungeonGimmickTable (DungeonId + AreaId)     (기믹 성공 조건, 역할 안내)
```

---

## 제약과 예외

- 절대 좌표와 실제 맵 배치 값은 맵 authoring 데이터에서 최종 확정한다.
- v1에서는 `PhaseTag = CORE`(1페이즈)와 `PhaseTag = EXTENSION`(2페이즈) 패턴 모두 구현 대상이다. 2페이즈 세부 패턴(소환 압박 강화 등)은 기본 구조 확정 후 보강한다.
- `CameraPullBackMeters`, `DurationMs` 같은 연출 수치는 프로토타입 후 미세 조정한다.
- 보스 2페이즈 구조(pending decision #1 확정)에 따라 `BossStaggerRuleTable.ResetOnPhase = true`로 2페이즈 전환 시 게이지를 초기화한다.
- `WarningTextKey`, `TextKey`, `RoleHintTextKey`, `MarkerKey`는 클라이언트 로컬라이제이션 리소스와 연결되는 계약 키다. 실제 문자열은 클라이언트 리소스 테이블에서 관리한다.

---

## 관련 문서

- [combat.md](./combat.md)
- [combat_ui_wireframe.md](./combat_ui_wireframe.md)
- [boss_pattern.md](./boss_pattern.md)
- [dungeon_progress.md](./dungeon_progress.md)
- [../common/data-table-conventions.md](../common/data-table-conventions.md)
