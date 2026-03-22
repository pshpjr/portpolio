# 세션 설계 — 소유권, 핸들러, 생명주기, 종료 처리

## 배경

게임 서버 네트워크 라이브러리를 설계하면서 세 가지 핵심 질문이 있었다.

1. **세션의 소유권을 누가 가질 것인가** — 라이브러리인가, 어플리케이션(컨텐츠)인가.
2. **패킷 수신 핸들러를 어떻게 등록할 것인가** — 상속, 템플릿, 콜백 중 어떤 방식을 쓸 것인가.
3. **세션 종료 시 OnDisconnect 이후 OnPacket이 호출되지 않음을 어떻게 보장하는가** — 스레드 모델과 종료 순서.

이 세 질문은 독립적으로 보이지만, 실제로는 하나의 일관된 설계 결정으로 연결된다.

---

## 세션 소유권 및 핸들러 등록 방식

### 검토한 선택지

#### 방식 1: 라이브러리가 세션 소유 + 가상 함수 상속

```cpp
// 라이브러리
class Session {
public:
    virtual void OnPacketReceived(PacketView pkt) = 0;
    virtual void OnConnected() {}
    virtual void OnDisconnected() {}
    void Send(std::span<const std::byte> data);
};

class ISessionFactory {
    virtual std::unique_ptr<Session> Create() = 0;
};

// 컨텐츠
class GameSession : public Session {
    User userData_;
    void OnPacketReceived(PacketView pkt) override { /* ... */ }
};
```

- **소유권**: 라이브러리의 NetworkService가 `unique_ptr<Session>`으로 소유.
- **장점**: 소유권 명확. 멀티포트 시 다중 세션 타입(GameSession + AdminSession)을 한 풀에 공존 가능.
- **미채택 이유**: 상속으로 라이브러리-컨텐츠가 결합된다. 세션과 유저 데이터의 생명주기가 강제 결합되어 분리가 어렵다.

#### 방식 2: 라이브러리가 세션 소유 + CRTP 정적 디스패치

```cpp
// 라이브러리
template <typename Derived>
class IoHandler {
protected:
    void DoRead() {
        static_cast<Derived*>(this)->OnPacketReceived(buffer_);
    }
};

template <typename SessionType>
class Acceptor { /* SessionType을 직접 생성 */ };

// 컨텐츠
class GameSession : public IoHandler<GameSession> {
    User userData_;
public:
    void OnPacketReceived(PacketView pkt) { /* ... */ }
};
```

- **소유권**: `Acceptor<S>`가 `shared_ptr<S>`로 관리. 세션 타입이 컴파일 타임에 고정.
- **장점**: 가상 함수 비용 0, 인라이닝 가능. 세션이 곧 유저 데이터 — 매핑 불필요.
- **미채택 이유**: **멀티포트 단일 풀 불가**. `IoHandler<GameSession>`과 `IoHandler<AdminSession>`은 다른 타입이라 공통 컨테이너에 넣을 수 없다. 게임 서버는 하나의 네트워크 시스템 안에서 모든 세션이 공존해야 하고, 어드민 세션에서 게임 세션에 접근하는 등의 상호작용이 필요하다. 이 제약이 순수 CRTP 방식을 배제하는 핵심 이유다.

#### 방식 3: 라이브러리가 세션 소유 + 콜백 등록 (채택)

```cpp
// 라이브러리 — 세션은 순수 네트워크 객체
class Session {
    void Send(/*...*/);
    SessionId GetId() const;
};

class NetworkService {
    using OnConnectFunc    = void(*)(Session*);
    using OnPacketFunc     = void(*)(Session*, PacketView);
    using OnDisconnectFunc = void(*)(Session*);

    void SetOnConnect(OnConnectFunc);
    void SetOnPacket(OnPacketFunc);
    void SetOnDisconnect(OnDisconnectFunc);
};
```

- **소유권**: 라이브러리가 세션 생명주기 관리. 컨텐츠는 세션 포인터를 받아서 유저와 매핑.
- **채택 이유**: 라이브러리-컨텐츠 계층 분리(상속 없음). 세션과 유저 데이터의 생명주기 독립.

### 세부 결정

#### std::function vs 함수 포인터

`std::function`은 힙 할당과 간접 호출 비용이 있고, 람다 캡처의 `this` 댕글링 위험이 있다. 함수 포인터는 상태를 캡처할 수 없지만, 글로벌 등록이면 상태 캡처가 필요 없다.

**결정: 함수 포인터 사용.**

#### 세션별 핸들러 vs 글로벌 핸들러

함수 포인터는 상태를 캡처할 수 없으므로, 세션별로 다른 동작을 하려면 결국 매핑 테이블을 거쳐야 한다. 세션별 핸들러 등록은 의미가 없다.

**결정: 글로벌 핸들러 하나만 등록. 세션별 분기는 컨텐츠 레이어에서 처리.**

#### 세션→유저 매핑 소유자

라이브러리의 Session 클래스는 네트워크 관심사만 담당해야 한다. 유저 데이터는 컨텐츠의 관심사이므로, 매핑 역시 컨텐츠가 관리하는 것이 자연스럽다.

**결정: 컨텐츠의 UserManager가 Session → User 매핑 관리.**

#### 상태별 핸들러 분리

유저 상태(로그인 전, 로비, 인게임 등)에 따라 허용되는 패킷이 다르다. 핸들러마다 상태를 체크하는 방식은 반복이 많고 "이 상태에서 뭐가 가능한지"를 한눈에 파악하기 어렵다. 상태별 HandlerTable을 정적으로 구성하고 포인터만 교체하는 방식을 선택했다.

```cpp
class User {
    const HandlerTable* currentHandlers_ = &g_loginHandlers;
public:
    void SetState(UserState state) {
        switch (state) {
            case UserState::Login:  currentHandlers_ = &g_loginHandlers;  break;
            case UserState::Lobby:  currentHandlers_ = &g_lobbyHandlers;  break;
            case UserState::InGame: currentHandlers_ = &g_ingameHandlers; break;
        }
    }
};
```

**결정: 상태별 HandlerTable을 전역으로 구성, 유저가 포인터로 현재 테이블을 참조.**

---

## 세션 생명주기와 상태 전이

### 상태 정의

```
Created → Negotiating → Active → Closing → Closed
```

- **Created**: 세션 객체 생성 직후.
- **Negotiating**: Accept 완료 후 프로토콜 판별 중. 5초 내 완료하지 못하면 종료.
- **Active**: 프로토콜 판별 완료, 정상 송수신 중.
- **Closing**: 종료 진행 중 (Send 큐 소진 또는 강제 종료 대기).
- **Closed**: 소켓 닫힘, 콜백 완료.

### 콜백 정의

| 콜백 | 호출 시점 | 반환값 | 비고 |
|---|---|---|---|
| `OnAccept(Session*)` | Accept 직후, Negotiating 진입 전 | `bool` | `false` 반환 시 즉시 종료 (IP 차단 등) |
| `OnConnect(Session*)` | 프로토콜 협상 완료, Active 진입 시 | `void` | 한 번만 호출 |
| `OnRecv(Session*, data, length)` | Active 상태에서 패킷 수신 시 | `void` | raw 바이트 전달 |
| `OnDisconnect(Session*, reason)` | 세션 종료 확정 시 | `void` | 한 번만 호출, strand context 보장 |

### 프로토콜 협상

- Negotiating 상태에서 최초 수신 바이트로 프로토콜을 판별한다.
- 지원 프로토콜: 커스텀 TCP (체크섬 + 길이 기반 헤더), HTTP.
- 판별 타임아웃: 5초. 초과 시 `DisconnectReason::NegotiationTimeout`으로 종료.
- Active 이후 프로토콜 전환은 없다. 다른 프로토콜 데이터가 들어오면 `ProtocolMismatch` 또는 `ProtocolViolation`으로 종료.
- 프레이밍과 프로토콜 판별 로직은 별도 `SessionHelper`가 담당하며, `OnRecv`는 raw 바이트만 전달받는다.

### isAlive

별도 `bool` 멤버 없이 `SessionState`로 계산한다.

```cpp
bool IsAlive() const {
    return state_ == SessionState::Active;
}
```

### 핵심 멤버 목록

| 멤버 | 타입 | 설명 |
|---|---|---|
| `sessionId_` | `SessionId` | 세션 고유 식별자 |
| `state_` | `SessionState` | 현재 상태 |
| `socket_` | `tcp::socket` | Asio 소켓 (strand 귀속) |
| `strand_` | `strand<executor>` | 모든 비동기 연산의 실행 컨텍스트 |
| `acceptedAt_` | `steady_clock::time_point` | Accept 시각 |
| `connectedAt_` | `steady_clock::time_point` | Active 진입 시각 |
| `disconnectReason_` | `DisconnectReason` | 종료 사유 |
| `recvBuffer_` | (ring buffer 등) | 수신 버퍼 |
| `sendQueue_` | (queue) | 송신 대기 큐 |
| `isSending_` | `atomic_flag` | 송신 진행 중 여부 (중복 send 방지) |
| `negotiationTimer_` | `steady_timer` | 협상 타임아웃 타이머 |

---

## 세션 종료 처리 — OnDisconnect 순서 보장

### 전제 조건

라이브러리 레벨에서 **serial recv**(한 번에 하나의 async_read만 pending)를 보장한다. recv 완료 핸들러가 "다음 recv를 걸지 말지" 결정하는 유일한 제어 지점이 된다.

### 기본 흐름

```cpp
void Session::OnRecvComplete(error_code ec) {
    if (ec || closed_) {
        onDisconnect_(shared_from_this(), disconnectReason_);
        return;
    }

    onRecv_(shared_from_this(), parsed_data, length);

    if (!closed_) {
        PostNextRecv();
    } else {
        onDisconnect_(shared_from_this(), disconnectReason_);
    }
}
```

serial recv이므로 `OnRecv`와 다음 recv 사이에 `closed_` 체크 기회가 항상 존재한다.

### 종료 경로

#### 경로 A — 네트워크 측 감지

상대방 연결 종료 또는 read 에러 → recv 완료 핸들러에서 에러 확인 → `OnDisconnect` 호출. 비교적 단순하다.

#### 경로 B — 컨텐츠 측 요청

컨텐츠가 `Session::Close()` 호출 → strand에 post → strand 내에서 소켓 닫힘 → recv 에러로 완료 → `OnDisconnect` 호출.

### 스레드 모델 — strand 전면 도입

#### 검토한 선택지

**Atomic counter + Close post 혼합:**

Session에 atomic counter를 두어 현재 걸려 있는 IO 횟수를 추적한다. counter를 음수로 만드는 스레드가 `OnDisconnect`를 호출하는 방식이다.

- **미채택 이유**: 이 세션 설계에는 recv, send에 더해 협상 타임아웃 **타이머**가 이미 존재한다. 비동기 연산이 recv/send/close에서 recv/send/timer/close로 늘어나면 카운터 규칙을 전면 재검증해야 한다. 새 이벤트가 추가될 때마다 같은 문제가 반복된다.

**Strand 전면 도입 (채택):**

세션의 모든 비동기 연산(recv, send, timer, Close)을 하나의 strand에 귀속시킨다. Strand는 같은 strand에 post된 핸들러가 절대 동시 실행되지 않음을 보장한다.

- **채택 이유**: 협상 타임아웃 타이머가 이미 3번째 비동기 연산으로 존재한다. Strand는 새 비동기 연산이 추가될 때 자연스럽게 대응하며, 직렬성이 strand 내부 메커니즘으로 보장된다. Atomic counter 대비 추론 부담이 낮다.

#### 최종 결정

| 항목 | 결정 |
|---|---|
| 스레드 모델 | **strand 전면 도입** |
| `Close()` 구현 | strand에 post (`io_context.post`가 아닌 `strand_.post`) |
| `OnDisconnect` 호출 스레드 | **항상 strand context** — io_context를 실행하는 스레드 중 하나 |
| 내부 파괴 순서 | `OnDisconnect` 리턴 → `shared_ptr` 참조 카운트 소멸 → 별도 동기화 불필요 |

```cpp
void Session::Close(DisconnectReason reason) {
    strand_.post([self = shared_from_this(), reason] {
        self->DoClose(reason);
    });
}
```

---

## 최종 아키텍처

```
┌──────────────────────────────────────────────────────┐
│                   네트워크 라이브러리                    │
│                                                      │
│  NetworkService                                      │
│  ├─ Session 생명주기 관리 (소유)                       │
│  ├─ Accept / Connect                                 │
│  └─ 글로벌 함수 포인터: OnAccept, OnConnect,           │
│                        OnRecv, OnDisconnect          │
│                                                      │
│  Session                                             │
│  ├─ strand_ (모든 비동기 연산 귀속)                    │
│  ├─ socket_, recvBuffer_, sendQueue_, isSending_     │
│  ├─ negotiationTimer_ (5초 협상 타임아웃)             │
│  ├─ state_, sessionId_, acceptedAt_, connectedAt_    │
│  └─ disconnectReason_                                │
└───────────────────────┬──────────────────────────────┘
                        │ Session* 전달
                        ▼
┌──────────────────────────────────────────────────────┐
│                   컨텐츠 레이어                        │
│                                                      │
│  글로벌 진입점                                         │
│  ├─ OnAccept(Session*)   → 연결 필터링 (bool 반환)     │
│  ├─ OnConnect(Session*)  → UserManager::CreateUser   │
│  ├─ OnRecv(Session*, data, length)                   │
│  │      → UserManager::Find(session)                 │
│  │      → user->GetHandlers()->Get(packetId)         │
│  │      → handler(user, pkt)                         │
│  └─ OnDisconnect(Session*, reason)                   │
│         → UserManager::RemoveUser                    │
│                                                      │
│  UserManager                                         │
│  └─ Session* → User* 매핑 관리                        │
│                                                      │
│  HandlerTable (상태별, 전역 정적)                      │
│  ├─ g_loginHandlers   : LOGIN_REQ                    │
│  ├─ g_lobbyHandlers   : ENTER_DUNGEON, CHAT, ...     │
│  └─ g_ingameHandlers  : MOVE, ATTACK, CHAT, ...      │
│                                                      │
│  User                                                │
│  └─ const HandlerTable* currentHandlers_             │
│     (상태 전이 시 포인터 교체)                          │
└──────────────────────────────────────────────────────┘
```

### 패킷 처리 흐름

```
strand 내 recv 완료:
    → SessionHelper가 raw 바이트 판별
        → OnRecv(Session*, data, length) 호출
            → UserManager에서 Session → User 조회
            → User의 현재 HandlerTable에서 패킷 ID로 핸들러 조회
            → 핸들러가 없으면 무시 (현재 상태에서 허용되지 않는 패킷)
            → 핸들러가 있으면 호출: handler(user, pkt)
```

---

## 설계 원칙 요약

- **라이브러리는 네트워크만 안다.** 유저, 게임 로직, 상태 같은 개념은 라이브러리에 존재하지 않는다.
- **컨텐츠가 매핑을 소유한다.** Session → User 매핑의 생성, 조회, 제거 모두 컨텐츠 책임.
- **함수 포인터로 글로벌 등록.** 세션별 핸들러 교체 없이, 컨텐츠 레이어의 디스패치 구조로 분기.
- **상태별 핸들러 테이블 분리.** 상태 전이 = 포인터 교체. 핸들러 내부의 상태 체크 제거.
- **strand로 모든 비동기 연산 직렬화.** recv, send, timer, Close 모두 동일한 strand에 귀속. 새 비동기 연산 추가 시 자동 적용.
- **OnDisconnect는 항상 strand context.** 종료 순서 보장이 strand에 의해 자동으로 유지됨.
- **확장은 필요할 때.** 권한 체크 등은 현재 구조가 막지 않으므로, 요구사항이 구체화된 후 추가.
