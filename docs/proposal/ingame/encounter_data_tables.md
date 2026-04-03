# 인카운터 데이터 테이블 구조

## 목적

보스 패턴, 전조, 체크포인트, 기믹 authoring에 필요한 테이블 구조를 정의한다.

## 핵심 규칙

- 패턴 자체와 전조/피드백은 분리한다.
- 디자이너가 `패턴 읽기`, `대응 수단`, `보상/실패`를 테이블 단위로 편집할 수 있어야 한다.
- 체크포인트와 기믹은 전투 테이블과 별도로 관리한다.

## 테이블 구조

### `BossPatternTable`

| 컬럼 | 설명 |
|---|---|
| `PatternId` | PK |
| `BossId` | 보스 ID |
| `PatternCode` | 내부 코드 |
| `DisplayName` | 표시 이름 |
| `PhaseTag` | `CORE`, `EXTENSION` |
| `TelegraphId` | FK -> `BossTelegraphTable` |
| `AnswerTag` | `PARRY`, `DODGE`, `INTERRUPT`, `SPREAD`, `FOCUS_BREAK` |
| `CooldownSec` | 쿨다운 |
| `StaggerDamage` | 무력화 기여량 |
| `Interruptible` | 인터럽트 가능 여부 |
| `FailurePenaltyTag` | 실패 시 효과 |
| `RewardWindowSec` | 대응 성공 후 딜타임 |

### `BossTelegraphTable`

| 컬럼 | 설명 |
|---|---|
| `TelegraphId` | PK |
| `AnimationCue` | 애니메이션 전조 키 |
| `GroundIndicatorShape` | 장판 형태 |
| `GroundIndicatorSec` | 장판 예고 시간 |
| `CastBarSec` | 시전 바 길이 |
| `SoundCue` | 오디오 경고 키 |
| `ScreenWarningTag` | 상단 경고 / 테두리 경고 등 |
| `CameraPullBackMeters` | 카메라 후퇴량 |

### `CombatFeedbackTable`

| 컬럼 | 설명 |
|---|---|
| `FeedbackTag` | PK |
| `TriggerEvent` | `PARRY_SUCCESS`, `PARRY_FAIL`, `INTERRUPT_SUCCESS`, `STAGGER_ENTER`, `ENRAGE_WARNING` |
| `ScreenPosition` | 표시 위치 |
| `DurationMs` | 지속 시간 |
| `SoundCue` | 오디오 키 |
| `Priority` | 우선순위 |
| `StackRule` | 중첩 규칙 |

### `DungeonCheckpointTable`

| 컬럼 | 설명 |
|---|---|
| `CheckpointId` | PK |
| `DungeonId` | 던전 ID |
| `AreaId` | 구역 ID |
| `TriggerCondition` | 체크포인트 저장 조건 |
| `RespawnAnchor` | 부활 위치 키 |
| `ResetRule` | 복귀 시 유지/초기화 규칙 |

### `DungeonGimmickTable`

| 컬럼 | 설명 |
|---|---|
| `GimmickId` | PK |
| `DungeonId` | 던전 ID |
| `AreaId` | 구역 ID |
| `GimmickType` | `ALTAR_SYNC`, `ESCORT` |
| `PartySize` | 파티 인원 |
| `RequiredCount` | 성공 필요 수 |
| `TimeLimitSec` | 시간 제한 |
| `FailureRecoverySec` | 복구 구간 길이 |
| `RetryRule` | 재시도 규칙 |
| `RoleHintTag` | 역할 안내 태그 |

## 제약과 예외

- 절대 좌표와 실제 맵 배치 값은 맵 authoring 데이터에서 최종 확정한다.
- v1에서는 `CORE` 패턴만 필수 구현 대상으로 둔다.
- `CameraPullBackMeters`와 `DurationMs` 같은 연출 수치는 프로토타입 후 미세 조정한다.

## 관련 문서

- [combat.md](./combat.md)
- [boss_pattern.md](./boss_pattern.md)
- [dungeon_progress.md](./dungeon_progress.md)
