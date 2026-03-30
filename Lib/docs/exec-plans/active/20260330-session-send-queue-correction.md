# Exec Plan: Session Send Queue Correction

## Goal

- 직전 세션 수정에서 합의와 다르게 들어간 `strand`/`deque` 기반 송신 구현을 제거한다.
- 세션 송신 경로를 `moodycamel::ConcurrentQueue`와 `isSending` 플래그 기준으로 다시 맞춘다.

## Scope

- `Lib/vcpkg.json`에 `concurrentqueue` 의존성 추가
- `session.h`, `session.cpp`에서 `strand` 제거
- 세션 송신 큐를 `moodycamel::ConcurrentQueue`로 교체
- send wake-up race가 없도록 `isSending`과 pending count 보강
- 기존 recv 버퍼 수정과 close idempotency는 유지

## Completion Criteria

- 세션 구현에 `boost::asio::strand`와 `std::deque` 기반 송신 코드가 남지 않는다.
- `Send()`는 `ConcurrentQueue`에 enqueue한 뒤 `isSending` 플래그로 단일 write chain만 시작한다.
- 비어 있는 큐로 전환되는 순간과 새 enqueue가 교차해도 송신이 멈추지 않는다.
- close/disconnect 경로가 중복 호출 없이 유지된다.

## Progress

- [completed] 사용자 피드백으로 구현 방향 불일치 확인
- [completed] send queue 설계를 합의 기준으로 재수정
- [completed] Lib/server manifest와 Lib CMake 패키지 연결 정리
- [pending] 사용자 확인 기준으로 빌드 검증

## Design Decision Log

- 송신 큐는 `moodycamel::ConcurrentQueue<std::shared_ptr<std::vector<char>>>`를 사용한다.
- 손실 없는 send wake-up을 위해 `isSending` 외에 pending count를 둔다.
- `strand`는 사용하지 않고, 세션 내부 동기화는 queue와 atomic 상태값으로 제한한다.
