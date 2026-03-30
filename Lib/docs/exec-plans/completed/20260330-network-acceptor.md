# Exec Plan: Lib network acceptor 구현

- **날짜**: 2026-03-30
- **상태**: completed
- **담당 영역**: `Lib/include/network/`
- **네임스페이스**: `psh::lib::network`

---

## 목표

`Lib/include/network/acceptor.cpp`의 미구현 구간을 채워 최소한의 수락기 수명주기와 세션 시작 흐름을 완성한다.
`acceptor`가 직접 의존하는 `Session` 인터페이스와 구현 간 불일치가 있으면, 빌드 가능성과 공개 계약 정합성을 해치지 않는 최소 범위에서 함께 정리한다.

---

## 범위

### 1. `acceptor` 수명주기 구현

- `Listen()`의 실행 순서를 정상화한다.
- `DisconnectAll()`과 `Stop()`의 기본 동작을 구현한다.
- 종료 중복 호출과 닫힌 acceptor 재진입을 방어한다.

### 2. 새 연결 처리 흐름 정리

- accept 완료 후 `Session` 생성과 시작 흐름을 연결한다.
- connect/recv/disconnect callback 연결 가능 범위를 정리한다.
- accept loop가 종료 상태에서는 재귀적으로 이어지지 않도록 한다.

### 3. 최소 정합성 수정

- `Session` 선언/정의 시그니처 불일치를 해소한다.
- `acceptor` 구현에 필요한 수준으로 `Session` callback 및 recv 시작 경로를 맞춘다.
- `Lib` 빌드 타깃에 `network/*.cpp`와 Boost.Asio 의존성을 편입한다.

---

## 구현 방침

- 기존 Boost.Asio 기반 구조는 유지한다.
- 서버 전용 정책이나 로깅 백엔드는 `Lib/`에 도입하지 않는다.
- 외부 의존성 추가 없이 현재 공용 라이브러리 범위 안에서 마무리한다.
- 공개 헤더 수정이 필요하면 `acceptor` 동작에 직접 필요한 최소 변경만 허용한다.

---

## 진행 단계

- [x] 1. `acceptor`와 `session` 연관 코드 조사
- [x] 2. `acceptor` 미구현 구간 및 실행 순서 수정
- [x] 3. 필요한 최소 `session` 정합성 수정
- [x] 4. 대상 빌드 또는 정적 검토로 검증
- [x] 5. 완료 시 active index 및 plan 상태 갱신

## 구현 메모

- `Session::StartRecv()`는 `async_read_some` 완료 시 `IoHandler`의 recv/disconnect callback을 호출하도록 연결했다.
- `Acceptor`는 세션 집합을 추적하고 `Stop()` 및 `DisconnectAll()`에서 일괄 종료를 수행한다.
- `Lib/CMakeLists.txt`를 실제 static library 타깃으로 전환하고 `boost-asio` / `Boost::asio` / `Boost::system` 의존성을 연결했다.
- `build/lib-debug`에서 `portpolio_lib_unit_tests` 빌드와 `ctest --output-on-failure` 실행까지 통과했다.

---

## 완료 기준

- `Acceptor::Listen()`, `DisconnectAll()`, `Stop()`, `StartAccept()`가 일관된 수명주기로 동작한다.
- 새 연결이 들어오면 세션이 생성되고 수신 시작 경로까지 연결된다.
- `acceptor`가 의존하는 `Session` 인터페이스/구현 불일치가 제거된다.
- 이번 작업에서 추가로 필요한 후속 작업이 있다면 명시적으로 기록한다.
