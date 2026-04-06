# Exec Plan: UClientNetworkSubsystem 기획 라운드

- **날짜**: 2026-04-06
- **범위**: office-hours — 클라이언트 네트워크 서브시스템 기획 문서 작성 및 리뷰
- **출력 대상**: `docs/proposal/client/network_subsystem.md`

## 배경

사용자 요구:
- `UClientNetworkSubsystem` (GameInstanceSubsystem) 설계
- 별도 스레드에서 소켓 I/O + 패킷 디코딩(추후)
- 수신 패킷을 큐에 push → 게임 스레드 tick에서 pop → 이벤트 발생
- 핵심 연산: `connect`, `disconnect`, `send`, `push`, `pop`

## 태스크

- [x] exec-plan 생성
- [x] `docs/proposal/client/` 폴더 및 `network_subsystem.md` 기획서 초안 작성 (design-author)
- [x] plan-eng-review: 서버/클라 계약, 상태 전이, 실패 흐름 검토 → pending-decisions #11~18 등재
- [x] architecture-eval: 스레드 모델 옵션 비교 → 옵션 A(raw bytes 큐) 채택, 기획서 반영
- [x] 기획서 P0 보강: Tick 소유권, FClientRemoteCharacterState 필드, 구역 전환 정책, Actor Send 경로 명확화
- [x] 서버 리뷰 반영: EntityType 필드 추가, TCP 전제·중복 패킷 방어 명시
- [x] OpenCode 3차 리뷰 반영: Protected 상태명 수정(death_revival.md 정합), Send() SPSC 경고, MoveTimestamp int64, Decode 실패 정책, Delegate 해제 순서, 미등록 EntityId 이동 패킷 방어, HP=0 전환 규칙, OnAllEntitiesCleared, TQueue unbounded 재정의
- [ ] performance-analysis: 큐 처리량 가설 및 tick 병목 시나리오 (선택적)
- [x] pending-decisions 갱신 (#11~18)

## 추가 범위 (2026-04-06 업데이트)

- `UClientWorldEntitySubsystem`도 같은 라운드에 포함: 나 외의 객체(다른 플레이어, 몬스터 등) 관리
- 두 서브시스템의 협력 계약(NetworkSubsystem → 패킷 dispatch → WorldEntitySubsystem 업데이트)도 문서화

## 범위 제약

- 이 라운드는 기획서 작성까지다. 언리얼 C++ 구현은 다음 라운드.
- 실제 `.proto` 메시지 정의는 `server/protocol.md`가 단일 소스. 여기서 중복 정의하지 않음.
