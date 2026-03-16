# AGENTS.md
> Service 레이어 작업 진입점.

## 이 디렉터리에서 먼저 확인할 것

1. `../../AGENTS.md`
2. `../../ARCHITECTURE.md`
3. `../../docs/conventions/cpp-style.md`

## Service 불변식

- Service는 여러 Core 시스템을 조합해 게임플레이 흐름을 만든다.
- Service는 `Types`, `Config`, `Core`만 직접 의존한다.
- 네트워크 프로토콜 세부사항은 Service에 넣지 않는다.

## 하위 도메인

- `dungeon/`: 인스턴스 생명주기와 진행
- `party/`: 파티 구성과 상태 전이
- `matchmaking/`: 매칭 규칙과 큐 관리

## 변경 시 체크

- Core에 있어야 할 순수 규칙이 Service로 새지 않았는지 확인
- 네트워크 패킷 형식이 서비스 로직에 박혀 있지 않은지 확인
- 상태 전이 테스트 또는 통합 테스트 필요성을 검토
