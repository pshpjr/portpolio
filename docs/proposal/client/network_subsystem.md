# 클라이언트 네트워크 서브시스템

> 이 문서는 클라이언트 측 네트워크 레이어와 월드 엔티티 관리의 **기획 계약**을 다룬다.
> 언리얼 C++ 구현 세부(헤더, UCLASS 매크로 등)는 구현 단계에서 확정한다.

---

## 범위

| 타입 | 종류 | 역할 |
|---|---|---|
| `UClientNetworkSubsystem` | GameInstanceSubsystem | 소켓 연결·수신·송신, 수신 큐 + 송신 큐 관리 (스레드 2개) |
| `UClientWorldEntitySubsystem` | GameInstanceSubsystem | 나 외 객체(타 플레이어·몬스터·NPC) 상태 보관 및 Actor 관리 |
| `FClientRemoteCharacterState` | 순수 구조체 (UObject 아님) | 패킷 역직렬화 결과 보관 |
| `AClientRemoteCharacter` | Actor | 원격 캐릭터 월드 표현, 보간 |

내 캐릭터는 `UClientOwnedCharacterSubsystem`이 담당한다 (기존 `UClientCharacterSubsystem`에서 이름 변경).

---

## UClientNetworkSubsystem

### 역할

게임 스레드와 소켓 I/O 스레드를 분리하고, 수신 패킷을 큐로 중계한다.

### 핵심 연산

| 연산 | 호출 주체 | 설명 |
|---|---|---|
| `Connect(host, port)` | 게임 스레드 | TCP 소켓을 열고 수신·송신 스레드를 시작한다 |
| `Disconnect()` | 게임 스레드 | 소켓을 닫고 수신·송신 스레드를 종료한다 |
| `Send(packet)` | 게임 스레드 | 직렬화된 패킷을 **송신 큐**에 push한다 (논블로킹) |
| `Push(packet)` | 수신 스레드 | 수신·(추후 디코딩된) 패킷을 수신 큐에 넣는다 |
| `Pop()` | 게임 스레드 Tick | 수신 큐에서 패킷을 꺼내 이벤트를 발생시킨다 |

### 스레드 모델

```
[게임 스레드]
  Send(packet) → 송신 큐 push (논블로킹)
  Tick: while Pop() → Decode + Dispatch

[소켓 송신 스레드]  (FRunnable + FRunnableThread)
  while 송신 큐 pop → send()

[소켓 수신 스레드]  (FRunnable + FRunnableThread)
  recv() → (추후: 디코딩) → 수신 큐 push
```

- **수신 큐**: `TQueue<FRawPacket, EQueueMode::Spsc>` — 수신 스레드(producer) / 게임 스레드(consumer)
- **송신 큐**: `TQueue<FRawPacket, EQueueMode::Spsc>` — 게임 스레드(producer) / 송신 스레드(consumer)
- 게임 스레드가 `send()`를 직접 호출하지 않으므로 소켓 I/O 지연이 Tick에 영향을 주지 않는다.

### 패킷 디코딩 전환 전략

#### 옵션 비교

| 축 | A: raw bytes 큐 (v1 채택) | B: 역직렬화 후 큐 (추후 전환 후보) |
|---|---|---|
| 수신 스레드 역할 | recv + 프레이밍만 | recv + 프레이밍 + Protobuf 역직렬화 |
| 게임 스레드 Pop() | Decode + Dispatch | Dispatch만 |
| 책임 경계 | I/O가 전송 계층에 한정 (우위) | I/O가 애플리케이션 스키마 의존 |
| 데이터 소유권 | 바이트 버퍼, 스레드 간 전달 단순 (우위) | 다형 객체 소유권 전이 필요 |
| 변경 비용 | 패킷 추가 시 수정 1곳 (우위) | 수정 2곳 (I/O + 게임 스레드) |
| 포트폴리오 적합성 | 낮은 위험, 단계적 판단 시연 (우위) | v1 범위에서 과설계 |

#### v1 결정: 옵션 A 채택

- v1 패킷 종류가 적어 게임 스레드 역직렬화 비용이 병목이 되지 않는다.
- I/O 스레드를 전송 계층에 한정하여 Protobuf 스키마 의존을 차단한다.
- Pop() 내부를 `Decode(raw)` → `Dispatch(decoded)` 두 단계로 분리하여, 전환 시 Decode 함수만 I/O 스레드로 이동할 수 있게 한다.

#### 전환 기준

| 단계 | 수신 스레드 | 게임 스레드 Pop() | 전환 조건 |
|---|---|---|---|
| v1 (현재) | raw bytes만 push | Decode + Dispatch | 기본 |
| v2 (조건부) | 디코딩 후 타입 태그와 함께 push | Dispatch만 수행 | 프로파일링에서 역직렬화 비용이 프레임 예산의 유의미한 비율 차지 시 |

전환 시 Push/Pop 시그니처 변경 없이 큐 원소 타입(`FRawPacket` → `FDecodedPacket`)만 교체한다.

### 연결 상태 전이

```
[Disconnected] → Connect() → [Connecting] → 성공 → [Connected]
[Connected] → Disconnect() 또는 소켓 에러 → [Disconnected]
```

- 에러 시 자동 재연결은 v1 범위 밖. 호출자가 재시도를 결정한다.
- 각 전이마다 `OnConnected`, `OnDisconnected(reason)` Delegate 발생.

### Tick 소유권 (P0)

**NetworkSubsystem이 Pop 루프를 단독으로 소유한다.**

```
NetworkSubsystem::Tick()
  while (수신 큐 Pop(raw))
    decoded = Decode(raw)            // 실패 시 → 에러 로그 + 해당 패킷 Skip
    if decoded.valid:
      OnPacketReceived.Broadcast(decoded.type, decoded.payload)
```

- `OnPacketReceived.payload` 타입: `TArray<uint8>` (역직렬화 이전 페이로드 바이트). 구독자가 타입에 맞게 Protobuf 파싱.
- WorldEntitySubsystem은 자체 Tick 없이 `OnPacketReceived` Delegate 구독만 한다. 보간은 Actor 자신의 Tick에서 처리한다.
- 이 구조로 두 서브시스템 간 Tick 순서 문제가 발생하지 않는다.
- Decode 실패(불완전 프레임, 스키마 불일치) 시: 에러 로그 후 해당 패킷만 Skip, 연결 유지. 연속 N회 실패 시 Disconnect (N은 구현 단계 결정).
- 한 Tick당 Pop 처리 상한은 미결 (→ pending-decisions #13). v1은 상한 없이 전부 처리.

> **Send() 스레드 제약**: `Send()`는 **게임 스레드에서만 호출**해야 한다. 송신 큐가 SPSC이므로 비동기 콜백(AsyncTask, HTTP 응답 등)에서 직접 호출하면 큐 데이터 손상이 발생한다. 비동기 콜백에서 패킷을 보내야 할 때는 게임 스레드로 마샬링(`AsyncTask(ENamedThreads::GameThread, ...)`) 후 호출한다.

### 이벤트 계약

| Delegate | 발생 조건 | 주요 구독자 |
|---|---|---|
| `OnPacketReceived(type, payload)` | Pop()에서 패킷 하나를 꺼낼 때마다 | WorldEntitySubsystem, 각 시스템 |
| `OnConnected` | 소켓 연결 성공 | UI, 로비 흐름 |
| `OnDisconnected(reason)` | 소켓 닫힘·에러 | UI, 세션 정리 |

---

## UClientWorldEntitySubsystem

### 역할

서버에서 내려오는 패킷을 받아 **나 외의 엔티티**(다른 플레이어, 몬스터, NPC)를 생성·갱신·제거한다.

**책임 목록**:
- 서버 패킷으로부터 엔티티 생성/삭제
- EntityId로 상태(`FClientRemoteCharacterState`) 조회
- `AClientRemoteCharacter` Actor 스폰/디스폰
- Actor와 상태 모델 연결
- 월드 레벨의 다른 유저/NPC/몬스터 전체 관리

### FClientRemoteCharacterState (순수 구조체)

패킷 역직렬화 결과를 담는 순수 데이터 컨테이너. `UObject`가 아니므로 GC 대상이 아니다.

| 필드 | 타입 | 설명 |
|---|---|---|
| `EntityId` | uint64 | 서버 부여 식별자 |
| `EntityType` | uint8 (enum) | `Player` / `Monster` / `NPC` — 스폰 시 Actor 종류 결정에 사용 |
| `TemplateId` | int32 | 캐릭터/몬스터 종류 ID |
| `Position` | FVector | 마지막 확인 위치 |
| `Rotation` | FRotator | 마지막 확인 방향 |
| `Level` | int32 | 레벨 |
| `DisplayName` | FString | 표시 이름 |
| `GuildName` | FString | 길드명 (없으면 빈 문자열) |
| `CurrentHP` | float | 현재 HP (타입은 전투 기획서 단일 소스) |
| `MaxHP` | float | 최대 HP (타입은 전투 기획서 단일 소스) |
| `Velocity` | FVector | 현재 이동 속도 벡터 (보간 품질에 필수, 공중 기믹 없으면 FVector2D 검토) |
| `MoveTimestamp` | int64 | 이동 패킷 기준 시각, 게임 세션 시작 이후 상대 시간(ms). float은 정밀도 부족(→ pending-decisions #17) |
| `CharacterState` | uint8 (enum) | `Alive` / `Dying` / `Dead` / `Reviving` / `Protected` |
| `CurrentSkillId` | int32 | 현재 시전 중인 스킬 ID (없으면 0) |
| `StatusEffectFlags` | uint32 | 활성 상태 이상 비트 플래그. 비트 매핑은 전투 기획서 단일 소스 |

**CharacterState 전이 규칙**

| 트리거 | 이전 상태 | 이후 상태 | 비고 |
|---|---|---|---|
| `DeathEvent` 수신 | Alive | Dying | HP=0 패킷만으로는 전환 안 함 (서버 권위) |
| 사망 연출 완료 (Actor 내부) | Dying | Dead | |
| `ReviveStateEvent(type=Start)` 수신 | Dead | Reviving | |
| `ReviveStateEvent(type=Complete)` 수신 | Reviving | Protected | HP = ReviveStateEvent.hp (서버 포함) |
| 보호 시간(2초) 만료 | Protected | Alive | 보호 중 피해 50% 감소 (death_revival.md 단일 소스) |

> **Protected 상태**: 무적(Invulnerable)이 아니라 **피해 50% 감소** 상태. `death_revival.md`가 단일 소스.  
> **HP 0 규칙**: 클라이언트는 `CurrentHP`가 0이 되어도 `DeathEvent`를 받기 전까지는 `Dying` 전환을 하지 않는다.

보스 전용 필드(무력화 게이지 등)와 장비 외형 정보 전달 범위는 미결 (→ pending-decisions #11, #14).

### AClientRemoteCharacter (Actor)

원격 캐릭터의 **월드 표현**을 담당한다.

**책임**:
- Skeletal Mesh, Animation Blueprint, 머리 위 UI Widget Component 보유
- `FClientRemoteCharacterState`를 받아 즉시 반영 또는 보간 처리
- 이동: 서버 위치를 목표로 클라이언트 측 보간 수행
- 사망/부활 상태 전환 시 애니메이션 트리거

### 패킷 처리 흐름

#### 스폰 패킷

```
NetworkSubsystem: Pop() → 타입 판별(RemoteCharacterSpawn)
  → WorldEntitySubsystem->OnRemoteCharacterSpawn(msg)
    1. FClientRemoteCharacterState 생성 (msg 역직렬화)
    2. AClientRemoteCharacter World에 Spawn
    3. Actor에 초기 State 반영
    4. Map<EntityId, {State, Actor}> 에 등록
```

#### 이동 패킷

```
NetworkSubsystem: Pop() → 타입 판별(RemoteCharacterMove)
  → WorldEntitySubsystem->OnRemoteCharacterMove(msg)
    1. Map에서 EntityId로 State 조회
    2. State.Position / Rotation / MoveTimestamp 갱신
    3. Actor->SetTargetTransform(newPos, newRot) 호출
    4. Actor 내부에서 Tick마다 보간 수행
```

#### 디스폰 패킷

```
NetworkSubsystem: Pop() → 타입 판별(RemoteCharacterDespawn)
  → WorldEntitySubsystem->OnRemoteCharacterDespawn(msg)
    1. Map에서 EntityId로 {State, Actor} 조회
    2. Actor의 OnPacketReceived Delegate 바인딩 해제  ← Destroy 전 반드시 먼저
    3. Actor->Destroy()
    4. Map에서 항목 제거
    5. OnEntityDespawned(entityId) Broadcast
```

### 핵심 연산

| 연산 | 호출 주체 | 설명 |
|---|---|---|
| `OnRemoteCharacterSpawn(msg)` | 게임 스레드 Tick (NetworkSubsystem 경유) | State 생성 + Actor 스폰 |
| `OnRemoteCharacterMove(msg)` | 게임 스레드 Tick | State 갱신 + Actor 이동 목표 전달 |
| `OnRemoteCharacterDespawn(msg)` | 게임 스레드 Tick | Actor 제거 + State 삭제 |
| `FindState(entityId)` | Actor, UI | 현재 State 포인터 반환 (없으면 nullptr) |

### 이벤트 계약

| Delegate | 발생 조건 | 주요 구독자 |
|---|---|---|
| `OnEntitySpawned(entityId)` | SpawnActor 성공 + Map 등록 완료 시 | 미니맵, HUD |
| `OnEntityDespawned(entityId)` | 개별 디스폰 완료 시 | 미니맵, 타게팅 시스템 |
| `OnAllEntitiesCleared` | Disconnect 또는 구역 전환으로 전체 초기화 시 | 미니맵 전체 리셋, 타게팅 해제 |

- SpawnActor 실패 시 `OnEntitySpawned`가 발생하지 않는다. Map에 등록되지 않으므로 이후 DespawnPacket이 와도 무시된다.

### 구역 전환 정책

- 구역 전환 트리거 패킷 수신 시 현재 Map의 모든 엔티티를 일괄 디스폰한다.
- 이후 서버가 새 구역의 스폰 패킷을 순차 발송한다.
- 구역 전환 중 플레이어 입력은 차단한다 (클라이언트 예측 없는 v1에서 입력 유실 방지).
- **전환 트리거 패킷, 빈 화면 처리(페이드/로딩), 스폰 완료 판정 방법은 미결** (→ pending-decisions #12, #15).

---

## 서브시스템 의존 방향

```
UClientNetworkSubsystem  (소켓·큐·Delegate)
    ↓ OnPacketReceived Delegate
UClientWorldEntitySubsystem  (State Map + Actor 관리)
    ↓ Delegate / 직접 호출
AClientRemoteCharacter  (월드 표현·보간)
```

- **역방향 참조 금지 범위**: WorldEntitySubsystem → NetworkSubsystem 직접 참조 없음.
- **Actor → NetworkSubsystem 참조는 허용**: Actor가 패킷을 보내야 할 때(예: 스킬 시전 요청) `GetGameInstance()->GetSubsystem<UClientNetworkSubsystem>()->Send()`를 직접 호출한다. 이는 역방향(Subsystem 간) 참조가 아닌 일반 게임 코드 → Subsystem 참조이므로 허용 범위다.

---

## 명칭 변경 사항

| 이전 이름 | 새 이름 | 이유 |
|---|---|---|
| `UClientCharacterSubsystem` | `UClientOwnedCharacterSubsystem` | WorldEntitySubsystem(타인 관리)과 명확히 구분 |

---

## 전제 조건

- **TCP 순서 보장 전제**: 이 설계는 TCP가 패킷 순서를 보장한다는 것을 전제한다. 순서 역전 보정이나 시퀀스 번호 기반 재정렬은 v1 범위 밖.
- **중복 스폰 방어**: 같은 EntityId로 SpawnPacket이 두 번 수신되면 두 번째는 무시하고 경고 로그를 남긴다.
- **미등록 EntityId 이동 패킷**: Map에 없는 EntityId로 MovePacket이 수신되면 무시 + 경고 로그. 구역 전환 직후 잔여 패킷에서 자주 발생할 수 있다.
- **디스폰 패킷 순서**: Map에 없는 EntityId의 DespawnPacket은 무시. Disconnect 시 모든 엔티티를 일괄 정리하며 이때 `OnEntityDespawned`를 개별 발생시키지 않고 별도 `OnAllEntitiesCleared` Delegate를 발생시킨다.
- **HP 0 전환 규칙**: 클라이언트는 `CurrentHP=0` 패킷을 받아도 `DeathEvent` 수신 전까지 Dying으로 전환하지 않는다. 사망 판정은 서버 권위.

---

## 미결 항목

- 패킷 프레이밍 규칙 (length-prefix 크기, byte order): `server/protocol.md`에서 확정.
- 보간 알고리즘 선택 (선형 보간 vs. dead reckoning): 구현 단계 결정.
- 몬스터/NPC 전용 타입이 필요하면 `AClientRemoteCharacter` 서브클래스 or 컴포넌트 분리: 구현 단계 결정.
- 클라이언트 예측 및 서버 Reconcile: v1 범위 밖.
- TLS/암호화: v1 범위 밖.
- 송신 실패 시 재시도 정책: v1은 에러 로그 + Disconnect.
- 송신 큐 메모리 고갈 방지: `TQueue`는 unbounded이므로 송신 스레드 블로킹 시 무한 증가 가능. v1은 상한 없이 운영하되 메모리 모니터링. 큐 상한을 둘지, 초과 시 Disconnect할지는 미결.

---

## 관련 문서

- [server/architecture.md](../server/architecture.md)
- [server/protocol.md](../server/protocol.md)
- [ingame/combat.md](../ingame/combat.md)
