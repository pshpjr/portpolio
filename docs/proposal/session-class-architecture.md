# Session Class Architecture (2026-04-08)

> `src/05_network/session/`에 들어갈 세션 클래스의 책임, 인터페이스 경계, 생명주기 설계.
> 네트워크 구현 결정([network-impl-decisions.md](network-impl-decisions.md))의 세션 관련 항목을 구체화한 문서.

---

## 1. 세션 클래스 책임 경계

세션 클래스는 네트워크 연결 생명주기와 I/O 상태를 관리하는 경계 객체이다.

**세션이 담당하는 것**: 연결 수락/거부, 상태 전이 관리, raw 바이트 수신 전달, send queue를 통한 송신, 종료 처리.

**세션이 담당하지 않는 것**: 패킷 의미 해석, 게임 로직, 인증/세션 검증, 패킷 직렬화/역직렬화.

---

## 2. SessionState 상태 전이

### 2.1 상태 enum

```cpp
enum class SessionState : uint8_t {
    Negotiating,  // Accept 완료, 프로토콜 판별 진행 중
    Active,       // 프로토콜 확정, 정상 통신 가능
    Closing,      // SendClose 호출됨, send queue drain 후 종료
    Closed        // 종료 완료
};
```

### 2.2 허용되는 전이

```
Negotiating ──→ Active ──→ Closing ──→ Closed
     │                        │
     │                        └──→ Closed (drain 중 I/O 에러)
     │
     │              Active ──→ Closed (비정상 종료)
     │
     └──→ Closed (타임아웃/에러, OnDisconnect 미호출)
```

| 전이 | 조건 | 동작 |
|---|---|---|
| Negotiating → Active | 프로토콜 판별 완료 | isNegotiated = true, OnConnect 호출 |
| Negotiating → Closed | 5초 타임아웃 또는 에러 | 소켓 정리만, OnDisconnect 미호출 |
| Active → Closing | SendClose 호출 | CAS 성공 시 OnDisconnect 호출, recv 중단, send drain |
| Active → Closed | 비정상 종료 (close 호출, I/O 에러, 원격 종료) | CAS 성공 시 OnDisconnect 호출 |
| Closing → Closed | send queue drain 완료 또는 drain 중 I/O 에러 | 소켓 정리 |

위 표에 없는 전이는 허용하지 않는다.

---

## 3. 종료 처리 핵심 로직

### 3.1 설계 원칙

- 세션별 strand를 사용하여 상태 전이, recv completion, close/sendClose를 직렬화한다.
- send 경로만 atomic_flag로 최적화한다 (enqueue는 strand 밖에서 발생).
- OnDisconnect 호출 여부는 `isNegotiated` flag로 결정한다.
- strand 사용 이유: 포트폴리오 가독성. "이 핸들러들은 동시에 실행되지 않는다"가 코드 구조에서 바로 보인다.

### 3.2 close() 흐름 (즉시 종료)

```
1. strand::post로 close 요청 진입
2. strand 내에서:
   if state != Active: return (이미 처리됨)
   state = Closed
   socket::close() 호출
       → 진행 중인 async_read / async_write가 operation_aborted로 취소
   isNegotiated 확인 → true면 OnDisconnect 호출
   recv buffer 미처리 데이터 폐기
```

### 3.3 sendClose() 흐름 (graceful 종료)

```
1. strand::post로 sendClose 요청 진입
2. strand 내에서:
   if state != Active: return (이미 처리됨)
   state = Closing
   OnDisconnect 먼저 호출 (상위 계층에 종료 알림, send는 best-effort)
   recv 중단 (새 async_read 시작하지 않음)
   send queue에 남은 데이터 flush 진행
   flush 완료 또는 flush 중 I/O 에러 시
       → state = Closed, 소켓 정리
```

### 3.4 Negotiating에서의 종료

Negotiating 상태에서 close()/sendClose()는 어플리케이션 레벨에서 호출할 수 없다.
해당 세션은 OnConnect가 불린 적이 없으므로 컨텐츠 측이 세션의 존재를 모른다.

Negotiating 중 끊김은 라이브러리/TCP 레벨에서만 발생한다:
- 타임아웃 (5초 내 프로토콜 미판별)
- TCP 에러
- 프로토콜 위반

이 경우 `isNegotiated == false`이므로 OnDisconnect를 호출하지 않고 소켓 정리만 수행한다.

### 3.5 동시성 보장 요약

| 관심사 | 보장 수단 |
|---|---|
| 상태 전이 단일 실행 | strand 직렬화 |
| OnDisconnect 단일 호출 | strand 내에서 1회 호출 + isNegotiated 확인 |
| send queue thread-safety | moodycamel ConcurrentQueue |
| 동시 write 방지 | atomic_flag isSending |
| 진행 중 I/O 취소 | socket::close() → operation_aborted |
| recv completion | strand에서 실행 |
| close/sendClose 진입 | strand::post로 직렬화 |
| Negotiating 종료 | strand 내부에서 처리 |

### 3.6 completion handler에서의 상태 확인

recv completion handler는 strand에서 실행되므로 상태 전이와 동시에 실행되지 않는다.
send completion handler는 strand 밖에서 실행되며, atomic_flag와 상태 확인으로 제어한다.

```
// recv completion handler (strand에서 실행)
on_recv_complete(error, bytes):
    if error == operation_aborted: return
    if state != Active: return
    // 정상 처리: OnRecv 호출, 다음 async_read 시작

// send completion handler (strand 밖, atomic_flag 제어)
on_send_complete(error, bytes):
    if error == operation_aborted: return
    if state == Closing:
        큐에 데이터 남아있으면 계속 전송
        큐가 비었으면 → strand::post로 Closed 전이 요청
        I/O 에러 시 → 남은 큐 폐기, strand::post로 Closed 전이 요청
        return
    if state == Closed: return
    // 정상: 큐에 데이터 남아있으면 계속 전송, 비었으면 isSending clear
```

---

## 4. 콜백 설계

**OnAccept**: 연결 정보 기반 승인 단계. `bool` 반환으로 세션 활성화 전 차단 여부를 결정한다. false 반환 시 세션은 생성되지 않는다.

**OnConnect**: 초기 프로토콜 판별이 완료되고 읽기/쓰기를 시작할 준비가 끝난 뒤 한 번만 호출한다. 이 시점에 isNegotiated를 true로 설정하고 상태가 Active로 전이된다.

**OnRecv(data, length)**: raw 바이트를 입력받는다. 프레이밍과 초기 프로토콜 판별은 SessionHelper가 담당한다.

**OnDisconnect**: 세션 상태가 Active에서 벗어날 때, CAS에 성공하고 isNegotiated가 true인 스레드가 단 한 번만 호출한다. 호출 시점은 상태 전이 이후이므로 콜백 내에서 DisconnectReason에 접근할 수 있다.

---

## 5. 프로토콜 판별

- 초기 프레이밍은 커스텀 체크섬과 길이 기반 헤더를 사용한다.
- 지원 프로토콜은 커스텀 TCP와 HTTP 두 가지로 제한한다.
- Accept 이후 Connect 전까지 Negotiating 상태, 5초 타임아웃.
- 프로토콜은 OnConnect 이전 협상에서 한 번만 결정, Active 이후 전환 불가.
- 결정 후 다른 프로토콜 데이터 수신 시 ProtocolMismatch / ProtocolViolation으로 종료.

---

## 6. 송신 경계

- `Send(buffer)`를 세션의 public 인터페이스로 노출.
- send queue는 moodycamel ConcurrentQueue 사용.
- Active 상태에서만 큐에 진입 가능, 그 외 상태에서 호출 시 무시(drop).
- `isSending` atomic_flag로 동시 async_write 방지: enqueue 후 flag가 false면 set하고 flush 시작, flush가 큐를 drain하다 빌 때 clear.
- Close 시 recv buffer 미처리 수신 데이터는 폐기.

---

## 7. SessionHelper

static function만을 담는 namespace 또는 struct 형태. 인스턴스 생성 불가.
프레이밍, 프로토콜 판별 등 세션이 직접 들고 있지 않아도 되는 순수 함수를 모아둔다.

---

## 8. DisconnectReason

네트워크 레이어 고유 사유(ProtocolMismatch, ProtocolViolation, Timeout, IOError 등)를 기본 포함.
상위 계층에서 끊은 경우 해당 사실을 저장하며, optional string으로 추가 사유를 내려줄 수 있다.

---

## 9. 세션 소유권

SessionManager가 세션 객체를 소유한다. 컨텐츠(상위 계층)에는 shared_ptr로 전달하며, 컨텐츠 측에서 수명을 연장하는 것을 허용한다.

설계 의도: SessionManager가 명확한 소유자이고, 컨텐츠는 borrowed reference로 사용한다.
shared_ptr은 소유권 분산이 아니라 수명 연장 가능성을 허용하기 위한 선택이다.

---

## 10. 헤더 인터페이스 초안

### 10.1 SessionState.h

```cpp
#pragma once
#include <cstdint>

namespace net {

enum class SessionState : uint8_t {
    Negotiating,
    Active,
    Closing,
    Closed
};

} // namespace net
```

### 10.2 DisconnectReason.h

```cpp
#pragma once
#include <optional>
#include <string>

namespace net {

enum class DisconnectCause : uint8_t {
    ProtocolMismatch,
    ProtocolViolation,
    NegotiationTimeout,
    IOError,
    RemoteClose,
    UpperLayerClose,      // 컨텐츠에서 close() 호출
    UpperLayerSendClose,  // 컨텐츠에서 sendClose() 호출
};

struct DisconnectReason {
    DisconnectCause cause;
    std::optional<std::string> detail;  // 상위 계층이 내려주는 추가 사유
};

} // namespace net
```

### 10.3 Session.h

```cpp
#pragma once
#include <atomic>
#include <cstdint>
#include <chrono>
#include <functional>
#include <memory>

#include "SessionState.h"
#include "DisconnectReason.h"

// forward declarations
namespace moodycamel { template<typename T> class ConcurrentQueue; }

namespace net {

using SessionId = uint64_t;
using Clock     = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Buffer    = /* TBD: buffer type */;

class Session {
public:
    // --- 식별 ---
    SessionId         id() const;
    SessionState      state() const;
    bool              isAlive() const;  // state == Active || state == Closing

    // --- 타임스탬프 ---
    TimePoint         acceptedAt() const;
    TimePoint         connectedAt() const;
    TimePoint         lastRecvTime() const;

    // --- 송신 인터페이스 ---
    // Active 상태에서만 큐 진입, 그 외 drop.
    void              send(Buffer buffer);

    // --- 종료 인터페이스 ---
    // close: CAS Active → Closed, 즉시 종료.
    void              close(DisconnectReason reason);

    // sendClose: CAS Active → Closing, send drain 후 종료.
    void              sendClose(DisconnectReason reason);

    // --- 콜백 타입 (생성 시 주입) ---
    using OnConnectFn    = std::function<void(Session&)>;
    using OnRecvFn       = std::function<void(Session&, const uint8_t* data, size_t length)>;
    using OnDisconnectFn = std::function<void(Session&, const DisconnectReason&)>;

private:
    // --- 식별 / 상태 ---
    SessionId                     sessionId_;
    SessionState                  state_{SessionState::Negotiating};  // strand 보호, atomic 불필요
    bool                          isNegotiated_{false};

    // --- 타임스탬프 ---
    TimePoint                     acceptedAt_;
    TimePoint                     connectedAt_;
    std::atomic<TimePoint>        lastRecvTime_;

    // --- 종료 사유 ---
    DisconnectReason              disconnectReason_;

    // --- I/O ---
    /* socket/channel handle - TBD */
    /* strand - TBD: asio::strand<executor_type> */
    /* recvBuffer - TBD */
    std::unique_ptr<moodycamel::ConcurrentQueue<Buffer>> sendQueue_;
    std::atomic_flag              isSending_{};

    // --- 콜백 ---
    OnConnectFn                   onConnect_;
    OnRecvFn                      onRecv_;
    OnDisconnectFn                onDisconnect_;
};

} // namespace net
```

### 10.4 SessionHelper.h

```cpp
#pragma once
#include <cstdint>
#include <optional>

namespace net {

enum class ProtocolType : uint8_t {
    CustomTcp,
    Http
};

struct SessionHelper {
    SessionHelper() = delete;

    static std::optional<ProtocolType> detectProtocol(
        const uint8_t* data, size_t length);

    static size_t parseFrameLength(
        const uint8_t* data, size_t length);

    static bool validateChecksum(
        const uint8_t* data, size_t length);
};

} // namespace net
```

---

## 11. 구현 전 검토 포인트

- negotiation timeout: 5초 타이머 관리 방식 (구현 시 결정)
- partial packet buffering: recvBuffer 관리와 프레이밍 연동 (구현 시 결정)
- Buffer 타입 정의 (구현 시 결정)
- 소켓/채널 핸들 타입 (asio 의존성에 따라 결정)

---

## 12. 테스트 전략

네트워크 라이브러리 테스트는 별도 문서로 분리하여 정리한다.

---

## 다음 단계

- 헤더 파일 작성 (Session.h, SessionState.h, DisconnectReason.h, SessionHelper.h)
- 소스 파일 구현
- 테스트 전략 문서 별도 작성
