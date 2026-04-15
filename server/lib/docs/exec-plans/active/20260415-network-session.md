# Exec Plan: Lib 네트워크 세션 골격 추가

- **날짜**: 2026-04-15
- **상태**: draft (검토 대기)
- **담당 영역**: `server/lib/include/network/`
- **네임스페이스**: `psh::lib::network` (신규)

---

## 1. 목표

서버·클라이언트 공용으로 쓸 네트워크 세션 프레임워크의 **헤더 골격**을 `Lib/`에 추가한다.
이번 plan은 구현(소스)이 아니라 아래 두 가지를 만드는 것을 목표로 한다.

1. Lib 관점에서의 네트워킹 범위 결정 (초안)
2. 공개 인터페이스 계약 문서 ([`../../network-session-interface.md`](../../network-session-interface.md))

실구현(`*.cpp`, asio 통합, 테스트)은 **다음 단계 plan**에서 분리하여 진행한다.

---

## 2. 상위 문서 참조

- 세션 클래스 설계 원본: [`docs/proposal/session-class-architecture.md`](../../../../../docs/proposal/session-class-architecture.md)
- 네트워크 구현 결정: [`docs/proposal/network-impl-decisions.md`](../../../../../docs/proposal/network-impl-decisions.md)
- 1차/2차 리뷰: [`docs/proposal/network-impl-review-r1.md`](../../../../../docs/proposal/network-impl-review-r1.md), [`network-impl-review-r2.md`](../../../../../docs/proposal/network-impl-review-r2.md)

본 plan은 위 문서의 결정을 **Lib 경계**로 옮겨 올 때 남는 공용 표면(헤더)에 한정한다.

---

## 3. Lib 경계 초안

### 3.1 Lib에 포함 — 공용 헤더 (프레임워크 계약)

`Lib/`가 "서버·클라이언트 공용"이 되려면 **런타임 바인딩이 없는 계약**만 둘 수 있다.
따라서 이번 단계에서는 다음 헤더만 `Lib/include/network/`에 추가한다.

| 헤더 | 책임 |
|------|------|
| `session_state.h` | `SessionState` enum |
| `disconnect_reason.h` | `DisconnectCause` enum, `DisconnectReason` 구조체 |
| `protocol_type.h` | `ProtocolType` enum (CustomTcp / Http) |
| `framing.h` | 패킷 헤더 레이아웃 상수, 체크섬, 길이 파싱 순수 함수 |
| `session_callbacks.h` | `OnAccept` / `OnConnect` / `OnRecv` / `OnDisconnect` signature typedef |
| `session_id.h` | `SessionId` 타입 별칭 (uint64) |

이 헤더들은 **표준 라이브러리만** 사용하며, asio·언리얼 의존성이 없다.
서버/클라가 같은 enum·함수 시그니처를 공유해 프로토콜 계약을 깨지 않게 하는 것이 목적이다.

### 3.2 Lib에 포함하지 않음 — 런타임 바인딩

다음은 `Lib/`에 두지 않고 각 앱에 둔다.

| 컴포넌트 | 배치 | 사유 |
|---|---|---|
| `Session` 클래스 본체 (asio strand, socket, send queue) | `server/src/network/` | asio 의존, 서버 전용 I/O 모델 |
| `SessionManager` | `server/src/network/` | 세션 풀·소유권, 서버 전용 |
| Unreal 측 세션 래퍼 | `client/` | 엔진 의존 |

### 3.3 경계에 대한 열린 질문

> `Lib/README.md`는 현재 "서버 런타임, 네트워크 의존 코드"를 Lib 제외 대상으로 명시한다.
> 이 plan은 **프로토콜 계약 헤더만** Lib에 들어가므로 README의 제외 대상과 충돌하지 않는다고 해석한다.
> 다만 서버·클라이언트 양측이 같은 프레임 포맷을 공유한다는 전제가 깨지면 Lib 배치 근거가 사라진다.

검토 필요:
- **Q1.** `session_callbacks.h`의 콜백 signature를 Lib에 두는 것이 옳은가, 서버 전용으로 낮추는 것이 옳은가?
- **Q2.** `framing.h`를 `psh::lib::network`에 둘지, 기존 `psh::lib::proto`에 합칠지?
- **Q3.** README의 "네트워크 제외" 문구를 "런타임 네트워크 구현 제외, 프로토콜 계약은 포함"으로 수정할지?

---

## 4. 파일 레이아웃 (초안)

```
server/lib/include/network/
├── README.md               ← 모듈 개요 + 인터페이스 문서 링크
├── session_state.h
├── disconnect_reason.h
├── protocol_type.h
├── session_id.h
├── session_callbacks.h
└── framing.h
```

상세 타입/함수 시그니처는 [`network-session-interface.md`](../../network-session-interface.md)에서 정의한다.

---

## 5. 범위 밖

- 실제 Session 클래스 구현 (asio 통합)
- SessionManager / 세션 풀
- 송수신 버퍼 구현
- 프로토콜 판별 로직 (헤더 시그니처만 정의, 구현은 별도 plan)
- 단위 테스트
- vcpkg / CMake 변경

위 항목은 후속 plan `20260416-network-session-impl.md`(가칭)에서 다룬다.

---

## 6. 진행 단계

- [ ] 1. 본 exec-plan 사용자 검토
- [ ] 2. 열린 질문 Q1–Q3 결정
- [ ] 3. 인터페이스 문서([`../../network-session-interface.md`](../../network-session-interface.md)) 확정
- [ ] 4. Lib AGENTS.md 네임스페이스 표에 `psh::lib::network` 등록
- [ ] 5. `server/lib/include/network/` 헤더 6종 작성 (다음 plan)
- [ ] 6. Lib README.md / include/README.md / docs/INDEX.md 갱신
- [ ] 7. active/INDEX.md 완료 처리 및 `completed/`로 이동

---

## 7. 완료 기준

- 인터페이스 문서가 고정되어 있다 (변경은 별도 plan 필요).
- Lib 경계 결정(Q1–Q3)이 문서에 기록되어 있다.
- 후속 구현 plan이 이 문서를 단일 소스로 참조할 수 있다.

---

## 8. 리스크 / 주의

- **설계 원본 변동**: `docs/proposal/session-class-architecture.md`는 여전히 단일 소스다.
  본 plan은 그 문서의 헤더 초안을 Lib 경계에 맞춰 재배치할 뿐, 새 설계 결정을 내리지 않는다.
  설계 변경이 필요하면 proposal 쪽을 먼저 고친다.
- **asio 유출 방지**: Lib 헤더에서 `asio::*` 타입을 절대 노출하지 않는다.
  `Buffer` 등 구현 의존 타입은 서버 쪽 헤더에서 정의한다.
- **언리얼 빌드 호환**: Lib 헤더는 `std::function`, `std::optional`, `std::chrono` 외 외부 의존을 넣지 않는다.
