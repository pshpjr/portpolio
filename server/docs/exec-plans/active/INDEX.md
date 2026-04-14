# Active Exec Plans
> 활성 실행 계획의 얇은 인덱스.
> 새 세션에서는 이 파일만 먼저 읽고, 관련 있는 plan 본문만 연다.

## 현재 활성 계획

- [20260414-boost-mysql-user-db-executor.md](./20260414-boost-mysql-user-db-executor.md) — boost-mysql 종속성 추가, User DB strand, IQuery, DbExecutor 도입.

## 운영 규칙

- 작업과 직접 무관한 활성 plan은 읽지 않는다.
- 작업 완료 후 관련 plan의 진행 상태만 갱신한다.
- 완료된 plan은 `completed/`로 이동한다.
