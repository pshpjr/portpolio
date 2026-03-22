# 세션 소유권 및 패킷 핸들러 등록 방식 설계 결정

## 배경

게임 서버 네트워크 라이브러리를 설계하면서 두 가지 핵심 질문이 있었다.

1. **세션의 소유권을 누가 가질 것인가** — 라이브러리인가, 어플리케이션(컨텐츠)인가.
2. **패킷 수신 핸들러를 어떻게 등록할 것인가** — 상속, 템플릿, 콜백 중 어떤 방식을 쓸 것인가.

이 두 질문은 독립적으로 보이지만, 실제로는 하나의 설계 결정이 두 가지로 나타나는 것이다. 세션을 누가 소유하느냐에 따라 핸들러 등록 방식이 자연스럽게 결정된다.

---

## 검토한 선택지

### 방식 1: 라이브러리가 세션 소유 + 가상 함수 상속

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
- **단점**: 상속으로 라이브러리-컨텐츠 결합. 세션과 유저 데이터의 생명주기가 강제 결합.

### 방식 2: 라이브러리가 세션 소유 + CRTP 정적 디스패치

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
- **단점**: **멀티포트 단일 풀 불가**. `IoHandler<GameSession>`과 `IoHandler<AdminSession>`은 다른 타입이라 공통 컨테이너에 넣을 수 없음.

> **멀티포트 관련 이전 논의**: 게임 서버는 하나의 네트워크 시스템 안에서 모든 세션이 공존해야 한다. 리스너를 따로 생성하면 사실상 두 개의 게임을 돌리는 것과 동일하고, 어드민 세션에서 게임 세션에 접근하는 등의 상호작용이 어려워진다. 이 제약이 순수 CRTP 방식을 배제하는 핵심 이유가 되었다.

### 방식 3: 라이브러리가 세션 소유 + 콜백 등록 (어플리케이션이 매핑 관리)

```cpp
// 라이브러리 — 세션은 순수 네트워크 객체
class Session {
    void Send(/*...*/);
    SessionId GetId() const;
};

class NetworkService {
    // 글로벌 함수 포인터로 이벤트 통지
    using OnConnectFunc    = void(*)(Session*);
    using OnPacketFunc     = void(*)(Session*, PacketView);
    using OnDisconnectFunc = void(*)(Session*);

    void SetOnConnect(OnConnectFunc);
    void SetOnPacket(OnPacketFunc);
    void SetOnDisconnect(OnDisconnectFunc);
};
```

- **소유권**: 라이브러리가 세션 생명주기 관리. 컨텐츠는 세션 포인터를 받아서 유저와 매핑.
- **장점**: 라이브러리-컨텐츠 계층 분리 (상속 없음). 세션과 유저 데이터의 생명주기 독립.
- **단점**: 세션 → 유저 매핑을 컨텐츠에서 별도 관리해야 함.

---

## 핵심 의사결정 과정

### 결정 1: 콜백 형태 — std::function vs 함수 포인터

`std::function`은 힙 할당과 간접 호출 비용이 있고, 람다 캡처의 `this` 댕글링 위험이 있다. 반면 함수 포인터는 상태를 캡처할 수 없지만, 글로벌 등록이면 상태 캡처가 필요 없다.

**결정: 함수 포인터 사용.**

### 결정 2: 세션별 핸들러 vs 글로벌 핸들러

함수 포인터는 상태를 캡처할 수 없으므로, 세션별로 다른 동작을 하려면 결국 매핑 테이블을 거쳐야 한다. 그렇다면 세션별 핸들러 등록은 의미가 없다.

**결정: 글로벌 핸들러 하나만 등록. 세션별 분기는 컨텐츠 레이어에서 처리.**

### 결정 3: 세션 → 유저 매핑은 누가 담당할 것인가

라이브러리의 Session 클래스는 네트워크 관심사만 담당해야 한다. 유저 데이터는 컨텐츠의 관심사이므로, 매핑 역시 컨텐츠가 관리하는 것이 자연스럽다. 이후 SessionID 기반으로 전달 방식을 변경해도 컨텐츠 레이어만 수정하면 된다.

**결정: 컨텐츠의 UserManager가 Session → User 매핑 관리.**

### 결정 4: 상태별 핸들러 분리

유저 상태(로그인 전, 로비, 인게임 등)에 따라 허용되는 패킷이 다르다. 각 핸들러 안에서 상태를 체크하는 방식도 가능하지만, 핸들러마다 반복되고 "이 상태에서 뭐가 가능한지"를 한눈에 파악하기 어렵다.

상태별 HandlerTable을 정적으로 구성하고, 유저의 상태 전이 시 테이블 포인터만 교체하는 방식을 선택했다.

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

### 결정 5: 권한 기반 확장 (GM 명령 등)은 열어둘 것인가

현재 구조에서 패킷 핸들러 호출의 진입점이 단일하므로, 나중에 권한 체크가 필요해지면 해당 지점에 한 겹을 끼워넣으면 된다. 요구사항이 구체화되기 전에 미리 추상화를 넣으면 불필요한 복잡도만 늘어난다.

**결정: 지금은 구현하지 않음. 현재 설계가 확장을 막지 않으므로 열어둘 필요 없음.**

---

## 최종 아키텍처

```
┌──────────────────────────────────────────────────────┐
│                   네트워크 라이브러리                    │
│                                                      │
│  NetworkService                                      │
│  ├─ Session 생명주기 관리 (소유)                       │
│  ├─ Accept / Connect                                 │
│  └─ 글로벌 함수 포인터: OnConnect, OnPacket, OnDisconnect │
└───────────────────────┬──────────────────────────────┘
                        │ Session* 전달
                        ▼
┌──────────────────────────────────────────────────────┐
│                   컨텐츠 레이어                        │
│                                                      │
│  글로벌 진입점                                         │
│  ├─ OnConnect(Session*)  → UserManager::CreateUser   │
│  ├─ OnPacket(Session*, PacketView)                   │
│  │      → UserManager::Find(session)                 │
│  │      → user->GetHandlers()->Get(packetId)         │
│  │      → handler(user, pkt)                         │
│  └─ OnDisconnect(Session*) → UserManager::RemoveUser │
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
IO 스레드: 패킷 수신 완료
    → NetworkService가 등록된 OnPacket 함수 포인터 호출
        → OnPacket(Session*, PacketView)
            → UserManager에서 Session → User 조회
            → User의 현재 HandlerTable에서 패킷 ID로 핸들러 조회
            → 핸들러가 없으면 무시 (현재 상태에서 허용되지 않는 패킷)
            → 핸들러가 있으면 호출: handler(user, pkt)
```

### 설계 원칙 요약

- **라이브러리는 네트워크만 안다.** 유저, 게임 로직, 상태 같은 개념은 라이브러리에 존재하지 않는다.
- **컨텐츠가 매핑을 소유한다.** Session → User 매핑의 생성, 조회, 제거 모두 컨텐츠 책임.
- **함수 포인터로 글로벌 등록.** 세션별 핸들러 교체 없이, 컨텐츠 레이어의 디스패치 구조로 분기.
- **상태별 핸들러 테이블 분리.** 상태 전이 = 포인터 교체. 핸들러 내부의 상태 체크 제거.
- **확장은 필요할 때.** 권한 체크 등은 현재 구조가 막지 않으므로, 요구사항이 구체화된 후 추가.
