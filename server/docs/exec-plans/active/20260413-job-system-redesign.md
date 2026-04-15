# Exec Plan: Job 시스템 인터페이스 재설계

> 생성: 2026-04-13
> 영역: `server/lib/include/job/`, `server/lib/src/job/`
> 인터페이스·데이터 명세: [`20260413-job-system-interface-spec.md`](./20260413-job-system-interface-spec.md)

## 목표

현재 `server/lib/include/job/` 의 `JobQueue` · `JobQueueHelper` · `TimedJobManager`(미구현) 삼각 결합을 해소하고, ASIO γ 스타일로 다음 세 컴포넌트를 분리한다.

- **`IExecutor`** — 함수 실행 엔진. 공개 API는 `bool Post(Callback)` 하나.
- **`JobQueue`** — 직렬 실행을 보장하는 job 큐. 자기 drain 클로저를 `executor.Post`로 스스로 스케줄. (※ ASIO strand 만큼 경량/범용이 아니라 자체 저장소·백프레셔·통계를 가진 **직렬 실행 JobQueue**. `strand` 라는 이름은 쓰지 않는다.)
- **`Timer`** — 시각 기반 job 관리. 생성 시 `IExecutor` 바인딩. 만료 시 `executor.Post` 또는 `queue.Post`로 재위임만.

세 컴포넌트는 서로를 최소한만 안다. Executor 는 JobQueue 를 모르고, Timer 는 queue target 을 `weak_ptr`로만 들고 있다.

## 배경 (현재 문제)

1. **JobQueue(=직렬 큐)가 Executor concrete 를 소유** — `job_queue.h:26` `CreateOptions::Executor`로 `WorkerPool` 직접 참조. 단위 테스트에 WorkerPool 필요, 실행 정책 교체 불가.
2. **`JobQueueHelper` overload 팽창** — `(Queue | Pool) × (PostAt | After | Repeat)` 6+ overload. (`job_queue_helper.h:22-120`)
3. **직렬 큐 없는 Post 경로 불명확** — "WorkerPool 대상 Helper overload"로만 표현됨.

## 확정된 설계 결정

| # | 결정 |
|---|---|
| D1 | ASIO γ 스타일. `IExecutor` + `JobQueue` + `Timer`. **별도 factory 함수 없이** `std::make_shared<T>(...)` + public 생성자 |
| D2 | Executor 공개 API: **`bool Post(Callback)`** 하나. `ScheduleStrand`/`ScheduleQueue` 같은 별도 API 없음 |
| D3 | `JobQueue` 는 `IExecutor` 를 `shared_ptr` 로 참조. 테스트는 `ThreadPoolExecutor` + 동기화 또는 `JobQueue::Pump/Flush` 사용 |
| D4 | Starvation 방지: BatchLimit 소진 시 queue 가 drain 클로저를 `executor.Post` 로 self-requeue |
| D5 | 이중 스케줄 방지: queue 내부 `running_` 플래그, `mtx_` 하에서 `queue_` 와 함께 관찰/수정 |
| D6 | Executor 종료: graceful drain + timeout. 초과 시 폐기 + 경고 로그 |
| D7 | Job 예외: swallow + log (안전망). 사용자 예외 금지 컨벤션 유지 |
| D8 | Timer 감시: 전용 스레드 1개 |
| D9 | Timer 자료구조: `unordered_map<bucket, list<shared_ptr<TimerEntry>>>` + `std::set<bucket>`. bucket = `floor(deadline / TickResolution)` |
| D10 | Timer 발화: 감시 스레드에서 콜백 직접 실행 금지, `executor.Post` / `queue.Post` 로만 재위임 |
| D11 | Executor 직통 `executor.Post(Callback) -> bool` 공식 API |
| D12 | JobQueue 백프레셔: `QueueCapacity` 초과 시 `Post` → invalid `JobHandle` |
| D13 | `JobQueue::Post` 는 **`JobHandle` 을 반환**하여 취소를 지원 (`bool` 아님). 실패 시 invalid handle. |
| D14 | **Entry 수명 모델**: `JobEntry` / `TimerEntry` 를 `shared_ptr` 로 컨테이너가 소유. `JobHandle` / `RepeatHandle` 은 `weak_ptr<Entry>` 로 참조. 상태·취소·일시정지·period/mode 는 Entry 의 atomic 필드. 별도 `idIndex_` 없음. |
| D15 | Timer target 수명: Timer 는 `shared_ptr<IExecutor>` 를 **생성 시 바인딩**. queue target 은 `weak_ptr<JobQueue>` 로 보유, 승격 실패 시 drop + debug log |
| D16 | Repeat: Timer 내부 재등록. FixedDelay=완료 기준, FixedRate=예약 시점 기준, catch-up 누적 없음 |
| D17 | `InlineExecutor` 는 두지 않는다. 결정적 테스트는 `JobQueue::Pump/Flush` 또는 `ThreadPoolExecutor` + 동기화 프리미티브로 수행 |
| D18 | `SlowJobOptions` 는 Executor 전역 옵션으로만 둔다 (JobQueue 옵션에서 제외) |
| D19 | `JobHandle` 은 `std::variant<monostate, weak<JobEntry>, weak<TimerEntry>>` 로 두 backend 통합. `RepeatHandle` 은 `weak<TimerEntry>` + `weak<Timer>`. `debugLabel/period/mode` 는 핸들 로컬 캐시 (읽기 전용). |
| D20 | Cancel 은 lazy: Entry 의 `Canceled` atomic 플립만 수행. 큐/버킷에서 실제 erase 하지 않고, dispatch/drain 이 플래그 관찰 후 skip. |
| D21 | 관측 API 는 컴포넌트마다 **`GetStatsSnapshot()` 하나로 단일화**. `RateStats` / `LatencySummary` / `HistogramSnapshot` 을 스냅샷 필드에 포함 (`Get*Stats` / `Get*Histogram` 별도 메서드 없음). |

## 범위

### 새로 생성
- `server/lib/include/job/executor.h` — `IExecutor`
- `server/lib/include/job/thread_pool_executor.h` — `ThreadPoolExecutor`
- `server/lib/include/job/timer.h` — `Timer` + `TimerEntry`
- `server/lib/src/job/thread_pool_executor.cpp`
- `server/lib/src/job/job_queue.cpp`
- `server/lib/src/job/timer.cpp`

### 수정
- `server/lib/include/job/job_queue.h` — **내용 완전 대체**. `JobEntry` + `JobQueue`. 파일 경로는 재사용.
- `server/lib/include/job/job.h` — `JobHandle` / `RepeatHandle` 을 `weak_ptr<Entry>` variant 기반으로 재설계 (공개 시그니처는 Cancel/State/Label 위주로 유지).
- `server/lib/include/job/job_queue_types.h` — `EntryId` 추가, `JobQueueStatsSnapshot` / `WorkerPoolStatsSnapshot` 에 `Rates` / `LatencySummary` / `Histogram` 필드 통합.
- `server/lib/include/clock/clock.h:4` — `namespace psh;::lib` → `namespace psh::lib` 오타 수정

### 제거
- `server/lib/include/job/job_queue_helper.h`

### 범위 외
- 우선순위 큐, 공정성 스케줄러 — 향후 확장(본 계획에서는 FIFO 가정)
- `io_context` 어댑터 — 필요 시 별도 plan
- 2단 계층 timing wheel — 활성 타이머 수가 커질 때 재검토
- `Job` 클래스 내부 백엔드 교체 이외의 공개 API 변경

## 단계

- [ ] **0. 사전 정리**
  - [ ] 0-1. 인터페이스·데이터 명세 문서 검토 완료 (`20260413-job-system-interface-spec.md`)
  - [ ] 0-2. `clock.h` 네임스페이스 오타 수정
  - [ ] 0-3. `job_queue_types.h` 에 `EntryId` 추가
- [ ] **1. Executor 계층**
  - [ ] 1-1. `IExecutor` 헤더
  - [ ] 1-2. `ThreadPoolExecutor` 최소 구현 (워커 1개, Post/Stop)
  - [ ] 1-3. 단위 테스트 (latch/promise 기반)
- [ ] **2. JobQueue**
  - [ ] 2-1. `JobEntry` + `JobQueue` 구현 (public 생성자, `shared_ptr<JobEntry>` 저장, `running_` 플래그)
  - [ ] 2-2. `JobQueue::Post` → `JobHandle` 반환 (weak<JobEntry>, 로컬 label 캐시)
  - [ ] 2-3. `JobHandle::Cancel` 백엔드 (JobEntry variant — lazy cancel)
  - [ ] 2-4. 단위 테스트 (`JobQueue::Pump/Flush` 기반, executor 불필요)
    - [ ] 직렬성 보장
    - [ ] Self-requeue (BatchLimit 소진)
    - [ ] Backpressure (QueueCapacity 초과 → invalid handle)
    - [ ] JobHandle Cancel (Queued → Canceled, lazy skip in DrainOnce)
    - [ ] 예외 흡수 후 생존
    - [ ] Stop(drain / timeout / force)
    - [ ] Executor 거절 시 JobQueue Stopped 전이
- [ ] **3. ThreadPoolExecutor (확장)**
  - [ ] 3-1. 워커 수 N, graceful shutdown 완성
  - [ ] 3-2. 단위 테스트 (`Post` FIFO, 동시성, Stop drain/timeout 의미)
  - [ ] 3-3. JobQueue × ThreadPoolExecutor 통합 테스트 (M queue × N worker, 각 queue 내부 순서 보존)
- [ ] **4. Timer**
  - [ ] 4-1. `FakeClock` 준비 (`IClock` 구현체)
  - [ ] 4-2. `Timer(executor, options)` public 생성자 (bucket/occupied/watcher 스레드, dispatch, `shared_ptr<TimerEntry>` 저장)
  - [ ] 4-3. `Timer::RequeueEntry` (Resume 재삽입 private helper)
  - [ ] 4-4. `JobHandle` TimerBackend / `RepeatHandle` 구현 (weak<TimerEntry> + weak<Timer>)
  - [ ] 4-5. 단위 테스트 (FakeClock 기반)
    - [ ] Bucket 경계 (동일 tick 내 엔트리 삽입 순서대로 발화)
    - [ ] Cancel (weak.lock() + atomic 플립)
    - [ ] Repeat FixedRate / FixedDelay (catch-up 누적 없음)
    - [ ] Pause/Resume (Pause 된 회차 drop, Resume 시 RequeueEntry)
    - [ ] ChangePeriod / ChangeMode 는 다음 회차부터 반영
    - [ ] JobQueue target weak 승격 실패 시 drop + Entry State=Canceled
    - [ ] 새 엔트리가 기존 최소 bucket 보다 이르면 watcher 재계산
- [ ] **5. Handle variant 완성**
  - [ ] 5-1. `JobHandle`: variant<monostate, weak<JobEntry>, weak<TimerEntry>> + 로컬 debugLabel
  - [ ] 5-2. Cancel / TryGetState / GetDebugLabel 라우팅 (CancelGeneric 공통 함수)
  - [ ] 5-3. `RepeatHandle` (Timer 전용, period/mode/label 로컬 캐시, Change* 는 성공 시 로컬 갱신)
- [ ] **6. 레거시 제거 및 마이그레이션**
  - [ ] 6-1. `JobQueueHelper` 참조 제거 (`grep` 0건)
  - [ ] 6-2. 기존 `job_queue_helper.h` 삭제, `job_queue.h` 내용 교체
  - [ ] 6-3. `server/CMakeLists.txt.full` 갱신
- [ ] **7. 빌드 및 검증**
  - [ ] 7-1. 전체 빌드 성공
  - [ ] 7-2. 모든 단위/통합 테스트 통과

## 완료 기준

- [ ] `server/lib/include/job/` 에 `JobQueueHelper` 참조가 남아있지 않다.
- [ ] `JobQueue` 단위 테스트가 `ThreadPoolExecutor` 없이 `JobQueue::Pump/Flush` 만으로 결정적으로 돌아간다.
- [ ] `Timer` 단위 테스트가 `FakeClock` 만으로 결정적으로 돌아간다.
- [ ] 통합 테스트(워커 N × queue M)에서 각 queue 내 실행 순서가 Post 순서와 일치한다.
- [ ] `JobHandle::Cancel()` 이 lazy 취소로 동작: 큐에서 실제 erase 하지 않고 DrainOnce 에서 skip 되는 것을 테스트로 확인.
- [ ] `server/docs/exec-plans/active/INDEX.md`가 이 plan 을 참조한다.

## 관련 문서

- 인터페이스·데이터 명세: [`20260413-job-system-interface-spec.md`](./20260413-job-system-interface-spec.md)
- 현재 레이아웃: `server/lib/include/job/{job.h, job_queue.h, job_queue_helper.h, job_queue_types.h}`
- 사전 논의: `server/lib/include/job_queue_impl_thoughts.md`, `server/lib/include/job/job-refactor-log.md`
- Clock 추상: `server/lib/include/clock/clock.h`
