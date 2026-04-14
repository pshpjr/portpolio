# Active Exec Plans
> 활성 실행 계획의 얇은 인덱스.
> 새 세션에서는 이 파일만 먼저 읽고, 관련 있는 plan 본문만 연다.

## 현재 활성 계획

- [20260414-login-move-sync-protocol.md](./20260414-login-move-sync-protocol.md) — 로그인/이동/유저 정보 동기화 프로토콜(.proto) 정의. (검토 대기)

## 운영 규칙

- 작업과 직접 무관한 활성 plan은 읽지 않는다.
- 작업 완료 후 관련 plan의 진행 상태만 갱신한다.
- 완료된 plan은 `completed/`로 이동한다.
