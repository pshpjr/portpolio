# Boost.Asio 네트워크 라이브러리 구현

- **상태**: 완료
- **생성**: 2026-04-16

## 목표

일반적인 Boost.Asio 기반 TCP 네트워크 라이브러리 구현.
`server/src/network/` 아래에 비동기 accept, read, write를 처리하는 핵심 클래스 구현.

## 범위

### 구현 대상

1. **`Session`** — 개별 TCP 연결 관리
   - `boost::asio::ip::tcp::socket` 래핑
   - 비동기 수신 (`async_read_some` + `RecvBuffer`)
   - 비동기 송신 (write queue + strand)
     - strand 대신 atomic flag로 send 중첩 막을 것
   - 세션 수명 관리 (`shared_from_this`)

2. **`Listener`** — TCP acceptor
   - `boost::asio::ip::tcp::acceptor` 래핑
   - 비동기 accept 루프
   - 새 연결 시 Session 생성

3. **`NetworkService`** — io_context 실행 관리
   - `io_context` + thread pool 실행
   - Listener 등록
   - 정상 종료(graceful shutdown)
     - 특정 세션 close 기능

### 기존 활용

- `RecvBuffer` — 그대로 사용
- `PacketHandler` — Session에서 수신 완료 시 호출

### 범위 밖

- 암호화/TLS
- 프로토콜 상세 파싱 (기존 packet_handler에 위임)
- Connection 상태 머신 (기존 Connection 클래스에 위임)

## 레이어 확인

- Network 레이어(`src/network/`)에 해당
- Types, Config, Core, Service만 참조 가능 (현재는 Types만 필요)

## 파일 목록

| 파일 | 작업 |
|------|------|
| `src/network/session.h` | 신규 |
| `src/network/session.cpp` | 신규 |
| `src/network/listener.h` | 신규 |
| `src/network/listener.cpp` | 신규 |
| `src/network/network_service.h` | 신규 |
| `src/network/network_service.cpp` | 신규 |

## 체크리스트

- [x] Session 비동기 read/write 구현
- [x] Listener 비동기 accept 구현
- [x] NetworkService io_context 관리 구현
- [x] 빌드 확인