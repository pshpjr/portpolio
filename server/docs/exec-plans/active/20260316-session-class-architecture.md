# Exec Plan: Session Class Architecture

## Goal

- `src/05_network/session/`에 들어갈 세션 클래스의 책임과 인터페이스 경계를 먼저 합의한다.
- 사용자가 제시한 `onAccept`, `onConnect`, `onRecv`, `onDisconnect`, `isAlive`, `lastRecvTime`을 기준으로 필수 멤버와 확장 포인트를 정리한다.
- 구현 전에 생명주기, 상태 관리, 송수신 경계, 상위 계층 의존성 방향을 아키텍처 결정으로 문서화한다.

## Scope

- 세션 클래스의 역할 정의
- 세션 생명주기 콜백과 상태 전이 설계
- 세션이 소유해야 하는 멤버와 외부로 위임해야 하는 책임 구분
- 이후 구현 시 필요한 검증 항목과 테스트 관점 정리

## Completion Criteria

- 세션 클래스의 최소 책임과 제외할 책임이 명확히 정리된다.
- 현재 제안된 인터페이스 외에 필요한 상태값, 식별자, 송신 경계, 종료 처리 항목이 문서 수준에서 합의된다.
- Network 레이어가 게임 규칙 계산을 직접 들고 가지 않도록 의존성 경계가 정리된다.
- 초기 프로토콜 판별, `OnConnect` 호출 조건, 프로토콜 고정 규칙이 명확히 정리된다.
- 다음 구현 단계에서 헤더와 소스 파일을 작성할 수 있을 정도로 의사결정이 구체화된다.

## Progress

- [completed] 관련 workflow, network 레이어 규칙, active exec-plan 규칙 확인
- [completed] 세션 클래스 아키텍처 결정용 plan 초안 작성 및 사용자 검토
- [completed] 사용자 피드백 반영
- [in_progress] 세션 클래스 설계안 확정
- [pending] 구현 단계로 전환

## Design Decision Log

- 세션 클래스는 네트워크 연결 생명주기와 I/O 상태를 관리하는 경계 객체로 두고, 게임 규칙이나 패킷 의미 해석은 별도 처리기로 위임한다.
- `OnAccept`는 연결 정보 기반 승인 단계이며, `bool` 반환으로 세션 활성화 전 차단 여부를 결정한다.
- `OnConnect`는 초기 프로토콜 판별이 완료되고 읽기/쓰기를 시작할 준비가 끝난 뒤 한 번만 호출한다.
- `OnRecv(data, length)`는 raw 바이트를 입력받고, 프레이밍과 초기 프로토콜 판별은 별도 `SessionHelper`가 담당한다.
- 초기 프레이밍은 커스텀 체크섬과 길이 기반 헤더를 사용한다. 현재 지원 프로토콜은 커스텀 `TCP`와 `HTTP` 두 가지로 제한한다.
- `Accept` 이후 `Connect` 전까지 세션은 `Negotiating` 상태로 두고, 5초 안에 프로토콜을 판별하지 못하면 종료한다.
- 한 세션의 프로토콜은 `OnConnect` 이전 협상 단계에서 한 번만 결정되며, `Active` 이후 다른 프로토콜로 전환하지 않는다.
- 전체 상태는 Created → Negotiating → Active → Closing → Closed 이다. 
- 프로토콜이 결정된 뒤 다른 프로토콜 데이터가 들어오면 네트워크 레벨 종료 사유(`ProtocolMismatch` 또는 `ProtocolViolation`)로 종료한다.
- `lastRecvTime`만으로는 세션 생존 판단 근거가 부족하므로 `acceptedAt`, `connectedAt`, `SessionId`, `SessionState`, 소켓/채널 핸들, `DisconnectReason`, `recvBuffer`, `sendQueue`, `atomic_flag isSending`를 핵심 멤버 후보로 본다.
- `isAlive`는 별도 저장 bool보다 상태값과 종료 상태를 기반으로 계산하는 방향을 기본안으로 둔다.
- 구현 전 검토 포인트는 다음과 같이 본다: thread-safety, disconnect idempotency, negotiation timeout, partial packet buffering, send queue ownership.

## 참고자료

- [세션 종료 시 처리 — OnCloseSession 이후 OnPacket 미호출 보장 논의 기록](../../docs/design/session-close-packet-ordering.md): serial recv 전제 하에 atomic counter 패턴과 Close() post 방식으로 경쟁 조건을 해결하는 설계 논의. strand 전면 도입 여부 미결.
