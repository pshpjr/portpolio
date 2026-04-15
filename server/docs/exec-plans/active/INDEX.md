# Active Exec Plans
> 활성 실행 계획의 얇은 인덱스.
> 새 세션에서는 이 파일만 먼저 읽고, 관련 있는 plan 본문만 연다.

## 현재 활성 계획

- [20260413-job-system-redesign.md](./20260413-job-system-redesign.md) - Job 시스템(Executor/Strand/Timer) 인터페이스 재설계 및 `JobQueueHelper` 제거
- [20260413-job-system-interface-spec.md](./20260413-job-system-interface-spec.md) - 위 plan의 인터페이스·데이터·내부 동작 명세 (단일 소스)

## 운영 규칙

- 작업과 직접 무관한 활성 plan은 읽지 않는다.
- 작업 완료 후 관련 plan의 진행 상태만 갱신한다.
- 완료된 plan은 `completed/`로 이동한다.
