# Exec Plan: Session Network Fixes

## Goal

- `Lib/include/network/`의 세션 구현에서 확인된 송수신, 패킷 파싱, 종료 처리 버그를 수정한다.
- 공개 API를 크게 흔들지 않으면서 공용 네트워크 세션이 부분 수신과 중복 종료를 안전하게 처리하도록 만든다.

## Scope

- `Packet::CheckLength`, `Packet::IsValid` 구현
- 세션 recv 버퍼 슬라이딩 처리 수정
- `Send()`를 send queue 기반 비동기 송신으로 전환
- `Close()`의 idempotency와 disconnect 알림 보장
- 관련 공개 헤더와 구현 파일 정리

## Completion Criteria

- 패킷 길이 검증이 실제 코드로 구현되어 세션 recv 경로에서 사용된다.
- 부분 패킷이 남았을 때 남은 바이트를 앞으로 당긴 뒤 다음 recv를 이어서 처리한다.
- 동시 `Send()` 호출이 직접 동기 write를 수행하지 않고 세션 내부 큐를 통해 직렬화된다.
- `Close()`가 중복 호출돼도 한 번만 정리되고 disconnect 알림이 일관되게 발생한다.

## Progress

- [completed] Lib 진입 문서, context-map, 활성 plan 인덱스 확인
- [completed] 세션 구현 리뷰 결과와 사용자 수정 방향 확인
- [completed] 세션 네트워크 수정 구현
- [completed] 사용자 확인 기준으로 빌드/최소 검증 완료

## Design Decision Log

- 프로토콜 판별은 현재 범위에서 제외하고, 세션은 raw packet framing과 연결 생명주기만 책임진다.
- 수신 버퍼는 고정 배열을 유지하되, 부분 소비 뒤 `memmove`로 남은 데이터를 앞으로 당기는 방식으로 단순하게 유지한다.
- 송신은 세션 내부 큐와 비동기 write 체인으로 직렬화한다.
- 종료는 `Close()` 한 곳에서 상태를 닫고, disconnect 콜백은 세션 단위로 한 번만 전달한다.
