# client/network_subsystem.md 클라이언트 구현 관점 리뷰

> 대상: `docs/proposal/client/network_subsystem.md`
> 날짜: 2026-04-06
> 관점: 클라이언트 화면 상태, 데이터 필드, 구역 전환, Tick 구조, 명칭 충돌

---

## 1. AClientRemoteCharacter 상태 전이 정의 부족

### 현재 상태

기획서는 `bIsDead` 불리언 하나로 사망을 표현하고, "사망/부활 상태 전환 시 애니메이션 트리거"라고만 적혀 있다.

### 기획서가 추가로 약속해야 하는 것

| 항목 | 필요 이유 |
|---|---|
| **원격 캐릭터 상태 enum 정의** (`Alive`, `Dying`, `Dead`, `Reviving`, `Invulnerable`) | `bIsDead` 불리언만으로는 사망 연출 중(Dying)과 확정 사망(Dead)을 구분할 수 없다. 부활 시전 중(Reviving)과 부활 직후 보호 상태(Invulnerable, 2초)도 별도 상태가 필요하다. |
| **각 상태 전이를 트리거하는 서버 패킷 타입** | `DeathEvent` 수신 -> Dying, 사망 연출 완료 -> Dead, `ReviveStateEvent` 수신 -> Reviving -> Alive 등 전이 조건이 서버 패킷 기준으로 명시되어야 한다. |
| **무력화(Stagger) 상태의 원격 캐릭터 표현** | combat.md는 보스 무력화 게이지와 다운 상태를 핵심 축으로 정의한다. 원격 캐릭터(파티원)가 무력화/경직 상태에 빠졌을 때 어떤 시각적 상태로 전이하는지 기획서에 없다. |
| **스킬 시전 중 상태** | 원격 캐릭터가 스킬을 시전하는 동안의 상태 표현(시전 모션 재생, 이동 불가 여부 등)이 정의되지 않았다. `SkillCastRequest`를 보낸 플레이어의 시전 상태가 다른 파티원에게 어떤 패킷으로 내려오는지 불명이다. |

---

## 2. FClientRemoteCharacterState에 누락된 필드

### 현재 필드 목록의 한계

기획서 자체가 "추가 필드는 전투 기획서 요구에 따라 확장한다"고 유보하고 있으나, combat.md, death_revival.md, combat_ui_wireframe.md에서 이미 요구하는 정보가 구체적이다. 유보가 아니라 지금 약속해야 한다.

### 기획서가 추가로 약속해야 하는 필드

| 필드 | 타입 | 요구 근거 |
|---|---|---|
| `Velocity` (속도 벡터) | FVector | 보간 알고리즘이 선형이든 dead reckoning이든 현재 이동 방향과 속력이 필요하다. Position + MoveTimestamp만으로는 패킷 간격이 불균일할 때 보간 품질이 급락한다. |
| `CharacterState` (상태 enum) | uint8 | 위 1번에서 정의한 상태 전이를 Actor에 전달하는 필드. |
| `CurrentSkillId` | int32 | 원격 캐릭터가 현재 시전 중인 스킬 ID. 애니메이션 블루프린트가 올바른 몽타주를 재생하려면 필수다. |
| `SkillCastProgress` | float (0~1) | 시전 진행도. 차지 스킬이나 긴 시전 스킬의 시각적 진행 표현에 필요하다. |
| `StaggerGauge` / `MaxStaggerGauge` | float | combat_ui_wireframe.md 상단에 보스 무력화 게이지가 항상 표시된다. 보스도 RemoteCharacter로 표현한다면 이 필드가 필요하다. 보스가 별도 타입이라면 그 분리 기준을 명시해야 한다. |
| `StatusEffects` | 비트 플래그 또는 배열 | 감전/중독/화상/빙결 4종의 활성 상태 이상이 원격 캐릭터 머리 위나 파티 UI에 표시되어야 한다. |
| `IsInvulnerable` | bool | 부활 직후 2초 보호 상태를 시각적으로 구분하기 위한 플래그. |
| `MoveSpeed` | float | 감전(-20%), 빙결(-40%) 등 상태 이상에 의한 이동속도 변화를 보간에 반영해야 한다. |

---

## 3. 구역 전환 시 빈 화면 처리 미명시

### 현재 기술

> "새 구역 진입 패킷 수신 시 현재 Map의 모든 엔티티를 일괄 디스폰한다. 이후 서버가 새 구역의 스폰 패킷을 순차 발송한다."

### 기획서가 추가로 약속해야 하는 것

| 항목 | 필요 이유 |
|---|---|
| **일괄 디스폰과 첫 스폰 패킷 사이의 화면 상태** | flush 직후 ~ 새 스폰 패킷 도착 전 구간에서 플레이어는 빈 월드를 보게 된다. 이 구간에 로딩 화면, 페이드 아웃/인, 또는 최소한 "구역 전환 중" UI를 표시하는지 결정이 필요하다. |
| **구역 전환 트리거 패킷의 정체** | `DungeonLifecycle` 그룹의 `CheckpointReachedEvent`가 구역 전환 트리거인지, 별도 `ZoneTransitionEvent` 패킷이 있는지 불명이다. protocol.md에도 구역 전환 전용 메시지가 없다. |
| **내 캐릭터(OwnedCharacter)의 구역 전환 처리** | WorldEntitySubsystem은 "나 외의 엔티티"만 담당한다. 구역 전환 시 내 캐릭터의 위치 리셋, 레벨 스트리밍/맵 전환 여부는 어느 서브시스템이 처리하는가? |
| **스폰 완료 판정 기준** | 서버가 "새 구역 스폰 패킷을 순차 발송"한다면, 클라이언트는 모든 엔티티가 도착했음을 어떻게 아는가? 스폰 완료 패킷(예: `ZoneReadyEvent`)이 있는지, 아니면 타이머 기반인지 약속이 필요하다. |
| **구역 전환 중 입력 차단 여부** | 빈 화면 구간에서 플레이어 이동/스킬 입력을 받을지 차단할지 결정이 필요하다. 차단하지 않으면 클라이언트 예측 없는 v1에서 입력이 유실된다. |

---

## 4. Tick 소유권: Pop 주체와 dispatch 구조 불명확

### 현재 기술의 모호성

- NetworkSubsystem: "Pop()은 게임 스레드 Tick에서 호출"
- WorldEntitySubsystem: "NetworkSubsystem 경유로 핸들러가 호출됨"
- 그러나 `OnPacketReceived` Delegate를 WorldEntitySubsystem이 구독한다고도 되어 있다.

### 기획서가 추가로 약속해야 하는 것

| 항목 | 필요 이유 |
|---|---|
| **Pop-dispatch의 단일 소유자 명시** | NetworkSubsystem이 자신의 Tick에서 `while Pop()`을 돌리며 Delegate를 fire하는 구조인지, 아니면 WorldEntitySubsystem이 자신의 Tick에서 NetworkSubsystem의 큐를 Pull하는 구조인지 하나만 확정해야 한다. 둘 다 Tick을 갖는 GameInstanceSubsystem이므로 Tick 순서가 보장되지 않으면 1프레임 지연이 생긴다. |
| **Tick 순서 보장 방법** | 언리얼 GameInstanceSubsystem의 Tick 순서는 등록 순서에 의존하며 명시적이지 않다. NetworkSubsystem -> WorldEntitySubsystem 순서를 보장하는 방법(예: WorldEntitySubsystem이 Tick을 쓰지 않고 Delegate만 수신, 또는 명시적 TickPrerequisite)을 기획 수준에서 결정해야 한다. |
| **한 Tick에 Pop되는 패킷 수 상한** | `while Pop()`이면 큐에 쌓인 패킷을 한 프레임에 전부 처리한다. 구역 전환 직후 대량 스폰 패킷이 한 프레임에 몰리면 프레임 스파이크가 발생한다. 프레임당 처리 상한 또는 분산 정책이 필요한지 결정해야 한다. |

---

## 5. UClientCharacterSubsystem 명칭 변경의 실제 충돌

### 현재 상황

기획서는 `UClientCharacterSubsystem` -> `UClientOwnedCharacterSubsystem`으로 이름 변경한다고 선언한다. 그러나 실제 코드베이스에서 `UClientCharacterSubsystem`은 이미 다음 파일에서 사용 중이다:

- `ClientCharacterSubsystem.h` / `.cpp` (클래스 정의)
- `ClientPlayerController.h` (주석 참조)
- `ClientStorageWidgetBase.cpp`
- `ClientSkillHotbarWidgetBase.cpp`
- `ClientInventoryWidgetBase.cpp`
- `ClientEquipmentWidgetBase.cpp`
- `ClientConsumableQuickSlotWidgetBase.cpp`

총 8개 파일, 16개 이상의 참조 지점이 있다.

### 기획서가 추가로 약속해야 하는 것

| 항목 | 필요 이유 |
|---|---|
| **리네임 시점과 방법** | 이 변경을 기획 문서 선언만으로 끝낼 수 없다. 코드 일괄 변경, 헤더 파일명 변경, generated.h 재생성이 필요하다. 구현 계획에 명시적 리네임 태스크로 잡아야 한다. |
| **WorldEntitySubsystem과 OwnedCharacterSubsystem의 책임 경계** | 내 캐릭터의 HP 변동, 상태 이상, 사망/부활은 누가 처리하는가? combat_ui_wireframe.md에서 "내 HP/자원/상태 이상"은 항상 표시해야 하는데, 이 데이터의 소유자가 OwnedCharacterSubsystem인지, 아니면 서버 패킷으로 WorldEntitySubsystem 경유인지 불명이다. |

---

## 6. 프로토콜 문서와의 패킷 타입 불일치

### 기획서가 추가로 약속해야 하는 것

| 항목 | 필요 이유 |
|---|---|
| **RemoteCharacterSpawn/Move/Despawn 패킷의 protocol.md 등재** | network_subsystem.md는 `RemoteCharacterSpawn`, `RemoteCharacterMove`, `RemoteCharacterDespawn` 세 패킷을 사용하지만, protocol.md의 메시지 그룹 어디에도 이 이름이 없다. `CombatState.ActorStateEvent`가 이를 포함하는 것인지, 별도 `EntitySync` 그룹이 필요한 것인지 확정해야 한다. |
| **보스 엔티티의 타입 구분** | protocol.md에 `BossPatternEvent`, `StaggerStateEvent` 등 보스 전용 패킷이 있다. 보스도 `RemoteCharacterSpawn`으로 생성되는 일반 엔티티인지, 별도 스폰 패킷을 쓰는지 결정이 필요하다. 이에 따라 `FClientRemoteCharacterState`에 보스 전용 필드를 넣을지 서브클래스를 만들지가 결정된다. |

---

## 7. 기타 누락 항목

| 항목 | 필요 이유 |
|---|---|
| **연결 끊김 시 원격 엔티티 처리** | `OnDisconnected` 발생 시 현재 Map의 모든 원격 엔티티를 즉시 제거하는지, 화면에 정지 상태로 남기는지 정의가 없다. |
| **재연결 시 상태 복원** | v1에서 자동 재연결이 범위 밖이라고 했지만, 수동 재연결 시 서버가 전체 스냅샷을 다시 보내는지, 클라이언트가 기존 Map을 유지하고 diff만 받는지 결정이 필요하다. |
| **파티원 전투 불능 상태의 원격 표현** | combat_ui_wireframe.md는 "파티원 전투 불능/부활 가능" 정보를 좌측 파티 UI에 항상 표시한다. 이 정보가 `FClientRemoteCharacterState`의 필드로 내려오는지, 별도 `PartyStateEvent`로 내려오는지 데이터 경로가 불명이다. |
| **Actor 역방향 참조의 Send 경로** | "Actor가 패킷을 보내야 할 때는 `NetworkSubsystem->Send()`를 직접 호출한다"는 문장이 바로 위의 "역방향 참조 금지" 원칙과 모순되지 않는지 확인이 필요하다. 역방향 참조 금지가 WorldEntitySubsystem -> NetworkSubsystem만 해당하는 것이라면, Actor -> NetworkSubsystem 참조는 허용인지 명시해야 한다. |

---

## 요약: 기획서 보강 요청 우선순위

| 우선순위 | 항목 | 이유 |
|---|---|---|
| **P0** | FClientRemoteCharacterState 필드 확정 (Velocity, 상태 enum, 스킬 ID, 상태 이상) | 모든 Actor 로직의 입력 데이터이므로 가장 먼저 확정 필요 |
| **P0** | 구역 전환 전용 패킷과 빈 화면 처리 정책 | 사용자가 매 구역마다 경험하는 전환이므로 후순위로 밀 수 없음 |
| **P0** | Pop-dispatch Tick 소유권 단일화 | 아키텍처 기반 결정이므로 구현 시작 전에 확정 필요 |
| **P1** | 원격 캐릭터 상태 전이 enum과 대응 패킷 | 사망/부활/무력화 애니메이션 전이의 전제 조건 |
| **P1** | protocol.md와 패킷 타입 정합 | 두 문서가 다른 이름을 쓰면 구현 시 혼란 발생 |
| **P2** | 명칭 변경 리네임 태스크 명시 | 기존 코드 8개 파일 변경이 필요한 실제 작업량 |
| **P2** | 연결 끊김/재연결 시 엔티티 정리 정책 | v1이라도 네트워크 불안정은 피할 수 없음 |
