# Network Session Interface (Lib)

> `server/lib/include/network/` 공개 헤더 계약 문서.
> 네임스페이스: `psh::lib::network`
> 동반 exec-plan: [`exec-plans/active/20260415-network-session.md`](./exec-plans/active/20260415-network-session.md)
> 설계 원본: [`docs/proposal/session-class-architecture.md`](../../../docs/proposal/session-class-architecture.md)

본 문서는 **Lib 경계에서 서버·클라이언트가 공유할 타입과 함수 시그니처**만 고정한다.
실행 모델(asio strand, socket lifetime, send queue)은 서버 전용이며 이 문서의 범위가 아니다.

---

## 1. 계약 범위

| 구분 | 포함 | 제외 |
|---|---|---|
| 타입 | enum, struct, 타입 별칭 | asio 타입, 엔진 타입 |
| 함수 | 순수 함수(프레이밍, 체크섬, 프로토콜 판별) | I/O, 소켓, 스레드 |
| 콜백 | signature typedef | 콜백 바인딩/디스패치 구현 |

---

## 2. 타입

### 2.1 `session_id.h`

```cpp
#pragma once
#include <cstdint>

namespace psh::lib::network {

using SessionId = std::uint64_t;

} // namespace psh::lib::network
```

### 2.2 `session_state.h`

```cpp
#pragma once
#include <cstdint>

namespace psh::lib::network {

enum class SessionState : std::uint8_t {
    Negotiating,  // Accept 완료, 프로토콜 판별 진행 중
    Active,       // 프로토콜 확정, 정상 통신 가능
    Closing,      // SendClose 호출됨, send queue drain 후 종료
    Closed        // 종료 완료
};

} // namespace psh::lib::network
```

허용 전이는 `Negotiating → Active → Closing → Closed`, `Negotiating → Closed`, `Active → Closed` 만 유효하다.
상세 전이 규칙은 설계 원본 §2.2 표를 단일 소스로 한다.

### 2.3 `protocol_type.h`

```cpp
#pragma once
#include <cstdint>

namespace psh::lib::network {

enum class ProtocolType : std::uint8_t {
    CustomTcp,
    Http
};

} // namespace psh::lib::network
```

프로토콜은 `Negotiating` 단계에서 한 번만 결정되며 `Active` 이후 전환 불가.

### 2.4 `disconnect_reason.h`

```cpp
#pragma once
#include <cstdint>
#include <optional>
#include <string>

namespace psh::lib::network {

enum class DisconnectCause : std::uint8_t {
    // 네트워크 라이브러리 자체 사유
    ProtocolMismatch,
    ProtocolViolation,
    NegotiationTimeout,
    IOError,
    RemoteClose,
    LargePacket,           // 16KB 초과 수신 (network-impl-decisions §1.1)

    // 상위 계층 사유
    UpperLayerClose,       // 컨텐츠 측 close() 호출
    UpperLayerSendClose,   // 컨텐츠 측 sendClose() 호출
    ContentRequest,        // 컨텐츠 로직에 의한 강제 종료
    DDoS                   // 과도한 요청 감지
};

struct DisconnectReason {
    DisconnectCause            cause;
    std::optional<std::string> detail;   // 상위 계층 추가 사유 (디버그/로그용)
};

} // namespace psh::lib::network
```

> `DisconnectReason`은 **서버 내부 로그용**이며 클라이언트에 전송하지 않는다
> (network-impl-decisions §1.1, §1.3).

---

## 3. 프레이밍

### 3.1 `framing.h`

패킷 헤더 레이아웃은 **little-endian, `[length: 2B][type: 2B][payload…]`**, `length`는 헤더 포함 전체 길이.
최대 패킷 크기는 16KB. (network-impl-decisions §1.1)

```cpp
#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>

namespace psh::lib::network {

struct PacketHeader {
    std::uint16_t total_length;   // 헤더 포함 전체 길이 (little-endian on wire)
    std::uint16_t type;           // 패킷 타입 ID
};

inline constexpr std::size_t kHeaderSize       = 4;    // sizeof(PacketHeader) on wire
inline constexpr std::size_t kMaxPacketBytes   = 16 * 1024;
inline constexpr std::size_t kMinPacketBytes   = kHeaderSize;

// 수신 버퍼 선두에서 헤더를 파싱한다.
// 길이 부족 시 std::nullopt 반환. 최대 크기 초과는 호출자가 DisconnectCause::LargePacket 로 처리.
std::optional<PacketHeader>
parse_header(const std::uint8_t* data, std::size_t length) noexcept;

// 헤더 기반 프레임 전체 길이 검사. total_length 가 [kMinPacketBytes, kMaxPacketBytes] 범위인지.
bool
is_valid_frame_length(std::uint16_t total_length) noexcept;

// 초기 바이트 시그니처로 프로토콜 판별.
// length 가 판별에 부족하면 std::nullopt 반환 (추가 수신 대기).
std::optional<ProtocolType>
detect_protocol(const std::uint8_t* data, std::size_t length) noexcept;

} // namespace psh::lib::network
```

> 구현은 헤더 전용(`inline`) 또는 별도 `framing.cpp`로 둘 수 있다.
> 체크섬 사용 여부는 설계 원본 §5에서 "커스텀 체크섬"을 언급하나,
> network-impl-decisions §1.1은 체크섬 필드를 포함하지 않는다.
> **열린 질문**: 체크섬 적용 여부 — decisions 쪽을 단일 소스로 두고 이 헤더에는 포함하지 않는다.

---

## 4. 콜백 시그니처

### 4.1 `session_callbacks.h`

세션 인스턴스 타입은 서버 쪽에서 정의되므로, Lib는 `Session` 타입을 **template parameter**로 받거나
**forward declaration**만 제공한다. 이번 단계에서는 forward declaration 방식을 채택한다.

```cpp
#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

#include "disconnect_reason.h"

namespace psh::lib::network {

class Session;  // forward: 구현은 server/src/network/session.h

using OnAcceptFn     = std::function<bool(SessionId)>;
using OnConnectFn    = std::function<void(Session&)>;
using OnRecvFn       = std::function<void(Session&,
                                          const std::uint8_t* data,
                                          std::size_t length)>;
using OnDisconnectFn = std::function<void(Session&, const DisconnectReason&)>;

} // namespace psh::lib::network
```

호출 계약 (설계 원본 §4):

| 콜백 | 호출 시점 | 호출 횟수 | 반환 의미 |
|---|---|---|---|
| `OnAcceptFn` | 연결 수락 직후, 세션 객체 활성화 전 | 0~1회 | `false` 반환 시 세션 미생성 |
| `OnConnectFn` | 프로토콜 판별 완료, Active 전이 시 | 정확히 1회 | — |
| `OnRecvFn` | 유효 프레임 수신 시 | 0회 이상 | — |
| `OnDisconnectFn` | 상태가 Active에서 벗어날 때, `isNegotiated == true` 조건 | 정확히 1회 | — |

---

## 5. 명시적으로 Lib에 두지 않는 항목

다음은 **서버 측 헤더**(`server/src/network/`)에서 정의하며, Lib 헤더는 이들에 대한 타입을 노출하지 않는다.

- `Buffer` 타입 (asio 또는 서버 전용 버퍼 구현에 결합)
- `Session` 클래스 본체 — strand, socket, send queue, 타임스탬프 저장
- `SessionHelper` 구현 — Lib의 `framing.h`를 조합해 서버에서 사용
- `SessionManager` — 세션 풀, shared_ptr 소유권

Lib에서 `Session&`를 forward declaration으로만 참조하는 것은 의도된 경계다.
콜백을 호출하는 쪽(서버)과 콜백을 작성하는 쪽(상위 로직)이 같은 타입 이름을 공유하게만 해 두고,
내부 멤버 접근은 서버 측 `session.h`에서만 가능하도록 한다.

---

## 6. 사용 예 (서버 측 시나리오)

```cpp
// server/src/network/session.h 에서
#include <psh/lib/network/session_state.h>
#include <psh/lib/network/disconnect_reason.h>
#include <psh/lib/network/session_callbacks.h>
#include <psh/lib/network/framing.h>

namespace psh::server::net {

class Session : public psh::lib::network::Session {   // 같은 이름의 완전 정의
    // asio strand, socket, send queue ...
public:
    void send(psh::server::net::Buffer buf);
    void close(psh::lib::network::DisconnectReason reason);
    void sendClose(psh::lib::network::DisconnectReason reason);
};

} // namespace psh::server::net
```

> 위 예시에서 Lib의 `Session` forward declaration과 서버의 완전 정의를 같은 이름으로 유지할지,
> 또는 서버가 Lib의 Session을 상속하지 않고 별 타입으로 두고 콜백에 `void*` 대신
> template 파라미터를 쓸지는 **exec-plan Q1** 에서 결정한다.

---

## 7. 버전 및 호환성

- 본 문서는 `v0` (초안). exec-plan 승인 후 `v1`로 태그.
- `SessionState`, `DisconnectCause`, `ProtocolType` enum 값은 **추가만 허용**, 기존 값 재정의 금지.
- `framing.h` 상수(`kHeaderSize`, `kMaxPacketBytes`)는 프로토콜 breaking change이므로 변경 시 별도 plan 필요.

---

## 8. 변경 이력

| 날짜 | 변경 | 링크 |
|---|---|---|
| 2026-04-15 | 초안 작성 | [exec-plan](./exec-plans/active/20260415-network-session.md) |
