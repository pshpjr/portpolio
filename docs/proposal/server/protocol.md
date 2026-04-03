# v1 프로토콜 범위

## 목적

v1 포트폴리오 슬라이스에서 어떤 메시지 그룹과 상태 전이가 필요한지 정의한다. 이 문서는 구현용 `.proto`가 아니라, 기획 기준의 메시지 범위와 책임만 다룬다.

## 핵심 규칙

- 클라이언트는 `요청`, 서버는 `응답`과 `브로드캐스트 이벤트`를 분리한다.
- 전투 판정, 무력화, 부활, 드롭은 서버 권위로 확정한다.
- v1에서는 메시지 그룹을 넓게 벌리지 않고, 핵심 플레이 루프에 필요한 범위만 정의한다.

## 메시지 그룹

### 인증 / 로비

| 그룹 | 예시 메시지 | 핵심 필드 |
|---|---|---|
| Auth | `LoginRequest`, `LoginResponse` | 계정 식별자, 세션 토큰 |
| Character | `CharacterListRequest`, `CreateCharacterRequest`, `SelectCharacterRequest` | 캐릭터 ID, 시작 무기 |
| TownState | `TownEnterEvent`, `InventorySnapshotEvent` | 현재 장비, 무기 프리셋, 소지 재화 |

### 파티 / 입장

| 그룹 | 예시 메시지 | 핵심 필드 |
|---|---|---|
| Party | `PartyCreateRequest`, `PartyInviteEvent`, `PartyStateEvent` | 파티 ID, 파티장 ID, 멤버 상태 |
| ReadyCheck | `DungeonReadyStartEvent`, `DungeonReadyVoteRequest`, `DungeonReadyResultEvent` | 원정 모드, 준비 상태, 차단 사유 |
| LoadoutLock | `LoadoutConfirmRequest`, `LoadoutLockedEvent` | 현재 무기, 스킬 프리셋, 전리품 선호 무기 |

### 인던 / 전투

| 그룹 | 예시 메시지 | 핵심 필드 |
|---|---|---|
| DungeonLifecycle | `DungeonEnterEvent`, `CheckpointReachedEvent`, `DungeonFailEvent` | 던전 ID, 구역 ID, 체크포인트 ID |
| CombatInput | `MoveInput`, `SkillCastRequest`, `ParryRequest` | 입력 방향, 스킬 ID, 타임스탬프 |
| CombatState | `ActorStateEvent`, `BossPatternEvent`, `CombatFeedbackEvent` | HP, 상태 이상, 현재 패턴, 피드백 태그 |
| Stagger / Interrupt | `InterruptSuccessEvent`, `StaggerStateEvent` | 대상 ID, 성공 여부, 남은 게이지 |
| Death / Revive | `DeathEvent`, `ReviveRequest`, `ReviveStateEvent` | 사망자 ID, 부활 방식, 보호 시간 |

### 보상 / 경제

| 그룹 | 예시 메시지 | 핵심 필드 |
|---|---|---|
| Loot | `LootGrantedEvent`, `LootPreferenceChangedRequest` | 아이템 ID, 수량, 전리품 선호 무기 |
| Economy | `RepairRequest`, `ReforgeRequest`, `TradeStateEvent` | 대상 아이템, 비용, 거래 가능 횟수 |
| Result | `DungeonRewardEvent`, `MissionResultEvent` | 원정 모드, 추가 보상 조건 충족 여부 |

## 상태 스냅샷 원칙

- 로비/마을에서는 큰 스냅샷 메시지를 허용한다.
- 전투 중에는 입력 요청과 상태 이벤트를 분리하고, 필요한 최소 정보만 자주 보낸다.
- UI가 즉시 반응해야 하는 항목은 별도 `FeedbackEvent`로 짧게 전달한다.

## 제약과 예외

- 실제 `.proto` 필드명과 직렬화 구조는 구현 문서에서 확정한다.
- 빈번한 좌표 동기화 주기, 압축 방식, 보간 규칙은 서버 기술 문서에서 확정한다.
- v1에서는 음성 채팅, 관전, 거래소 메시지 그룹을 다루지 않는다.

## 관련 문서

- [architecture.md](./architecture.md)
- [performance_test.md](./performance_test.md)
- [../outgame/party.md](../outgame/party.md)
- [../ingame/combat.md](../ingame/combat.md)
