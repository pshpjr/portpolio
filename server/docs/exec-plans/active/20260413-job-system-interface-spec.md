# 인터페이스 · 데이터 명세: Job 시스템 (`IExecutor` / `JobQueue` / `Timer`)

> 생성: 2026-04-13
> 범위: `server/lib/include/job/`, `server/lib/src/job/`
> 상위 plan: [`20260413-job-system-redesign.md`](./20260413-job-system-redesign.md)

이 문서는 **인터페이스와 내부 데이터·동작의 계약**을 기술한다. 구현 시 이 문서가 단일 소스이며, 여기에 기술되지 않은 동작은 구현 전 본 문서를 먼저 갱신한다.

---

## 용어

- **Executor**: 함수 실행 엔진. 스레드풀의 추상.
- **JobQueue**: 직렬 실행 큐. 자체 저장소·백프레셔·통계를 가지며 drain 클로저를 Executor 에 Post 해서 실행 슬롯을 확보. (ASIO strand 처럼 저장소 없는 경량 뷰가 아니다.)
- **Timer**: 시각 기반 Job 관리. 생성 시 Executor 바인딩. 만료 시 Executor 또는 JobQueue 로 재위임.
- **Entry**: `JobEntry` / `TimerEntry`. 컨테이너가 `shared_ptr` 로 소유하며, Handle 은 `weak_ptr` 로 참조.
- **Handle**: `JobHandle` / `RepeatHandle`. Entry 의 atomic 필드를 통해 취소·일시정지·설정 변경을 수행.

---

## 0. 공통 타입

`server/lib/include/job/job_queue_types.h`:

```cpp
namespace psh::lib::job
{
using TimePoint = std::chrono::steady_clock::time_point;
using Duration  = std::chrono::nanoseconds;

enum class EnumJobState       { WaitingTimer, Queued, Executing, Done, Failed, Canceled, Rejected };
enum class EnumJobQueueState  { Running, Draining, Stopped };
enum class EnumRepeatMode     { FixedRate, FixedDelay };
enum class EnumCancelResult   { Canceled, AlreadyCanceled, AlreadyExecuting, AlreadyFinished, Expired };

struct SlowJobOptions    { Duration WarningThreshold{0}; bool EnableWarningLog = false; };
struct HistogramBucket   { Duration LowerBound, UpperBound; uint64_t Count = 0; };
struct HistogramSnapshot { std::vector<HistogramBucket> Buckets; bool IsApproximate = true; };
struct RateStats         { double SubmittedPerSecond, ExecutedPerSecond, CompletedPerSecond, FailedPerSecond; };
struct LatencySummary    { Duration Average, Maximum, P50, P95, P99; };

// 모든 관측 지표를 단일 스냅샷에 통합. 외부는 GetStatsSnapshot() 만 사용.
struct JobQueueStatsSnapshot
{
    EnumJobQueueState State = EnumJobQueueState::Stopped;

    uint64_t PendingCount         = 0;
    uint64_t SubmittedCount       = 0;
    uint64_t ExecutedCount        = 0;
    uint64_t FailedCount          = 0;
    uint64_t RejectedCount        = 0;
    uint64_t CanceledCount        = 0;
    uint64_t BatchLimitHitCount   = 0;
    uint64_t DrainRescheduleCount = 0;

    RateStats         Rates{};
    LatencySummary    WaitTime{};
    LatencySummary    ExecutionTime{};
    HistogramSnapshot WaitHistogram{};
    HistogramSnapshot ExecutionHistogram{};
};

struct WorkerStateSnapshot
{
    uint32_t ThreadIndex = 0;
    bool     IsRunning   = false;
    bool     IsIdle      = false;
    std::optional<std::string> CurrentLabel;
};

struct WorkerPoolStatsSnapshot
{
    EnumJobQueueState State = EnumJobQueueState::Stopped;

    uint32_t WorkerCount    = 0;
    uint64_t PendingCount   = 0;
    uint64_t SubmittedCount = 0;
    uint64_t ExecutedCount  = 0;
    uint64_t FailedCount    = 0;
    uint64_t RejectedCount  = 0;

    RateStats         Rates{};
    LatencySummary    QueueWaitTime{};
    LatencySummary    ExecutionTime{};
    HistogramSnapshot QueueWaitHistogram{};
    HistogramSnapshot ExecutionHistogram{};

    std::vector<WorkerStateSnapshot> Threads;
};

using EntryId = uint64_t;   // 컴포넌트별 단조 증가 id. 디버그/로그 용도.
}
```

---

## 1. `IExecutor`

`server/lib/include/job/executor.h`:

```cpp
class IExecutor
{
  public:
    using Callback = std::function<void()>;

    virtual ~IExecutor() = default;

    virtual bool              Post(Callback fn)                                       = 0;
    virtual EnumJobQueueState GetState()                                    const noexcept = 0;
    virtual void              Stop(bool drain = true, Duration drainTimeout = Duration::zero()) = 0;
    virtual WorkerPoolStatsSnapshot GetStatsSnapshot()                       const   = 0;
};
```

### 계약


| 멤버                   | 보장                                                                                                                                     |
| ---------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `Post(fn)`             | 정상`true`. 포화/종료 상태 `false`. 실패 시 `fn` 은 호출되지 않고 소멸.                                                                  |
| `GetState()`           | `Running` / `Draining` / `Stopped` 중 하나.                                                                                              |
| `Stop(drain, timeout)` | `Running → Draining → Stopped`. `drain=true` 시 기존 큐 소진까지 블록하며 timeout 초과 시 폐기 + 경고. `drain=false` 시 큐 즉시 clear. |

### 불변조건

- 상태 전이는 단방향. `Stopped` 이후 `Post` 는 항상 `false`.
- `Post` 가 `false` 를 반환하면 외부 관측 가능한 부작용이 없다.
- JobQueue / Timer 를 알지 못한다. 스케줄은 모두 `Post(Callback)` 로 수렴.

---

## 2. `ThreadPoolExecutor`

`server/lib/include/job/thread_pool_executor.h`:

```cpp
class ThreadPoolExecutor final
    : public IExecutor,
      public std::enable_shared_from_this<ThreadPoolExecutor>
{
  public:
    struct CreateOptions
    {
        uint32_t       WorkerCount        = 0;   // 0 → std::thread::hardware_concurrency()
        uint32_t       ReadyQueueCapacity = 0;   // 0 → unbounded. 초과 시 Post → false
        SlowJobOptions SlowJob{};
        std::string    DebugName;
    };

    explicit ThreadPoolExecutor(const CreateOptions& options);
    ~ThreadPoolExecutor() override;

    ThreadPoolExecutor(const ThreadPoolExecutor&)            = delete;
    ThreadPoolExecutor& operator=(const ThreadPoolExecutor&) = delete;

    bool                    Post(Callback fn)                                override;
    EnumJobQueueState       GetState()                        const noexcept override;
    void                    Stop(bool drain, Duration drainTimeout)          override;
    WorkerPoolStatsSnapshot GetStatsSnapshot()                const           override;

  private:
    void WorkerLoop(uint32_t index);
};
```

> `enable_shared_from_this` 사용 특성상 `std::make_shared<ThreadPoolExecutor>(opts)` 로 생성해야 한다. 스택/`unique_ptr` 생성 시 `shared_from_this()` 호출은 정의되지 않은 동작.

### 내부 상태

```cpp
std::mutex               mtx_;
std::condition_variable  cv_;
std::deque<Callback>     ready_;
EnumJobQueueState        state_       = Running;
bool                     drainOnStop_ = false;
std::vector<std::thread> workers_;
CreateOptions            options_;
```

### 동작

```
Post(fn):
    lock(mtx_)
    if state_ != Running                                         return false
    if ReadyQueueCapacity > 0 && ready_.size() >= ReadyQueueCap  return false
    ready_.push_back(move(fn)); stats.submitted++
    cv_.notify_one(); return true

WorkerLoop(i):
    while true:
        Callback fn
        { lock(mtx_)
          cv_.wait(lock, [] return !ready_.empty() || state_ != Running)
          if ready_.empty():
              if state_ == Stopped                    return
              if state_ == Draining && !drainOnStop_  return
              continue
          fn = move(ready_.front()); ready_.pop_front() }
        t0 = now; try { fn() } catch(...) { log; stats.failed++ }
        if SlowJob.Enable && (now - t0) > SlowJob.WarningThreshold: log_warn
        stats.executed++

Stop(drain, timeout):
    { lock; state_ = Draining; drainOnStop_ = drain
      if !drain: ready_.clear()
      cv_.notify_all() }
    if drain:
        lock
        cv_.wait_until(lock, deadline, [] return ready_.empty())
        if !ready_.empty(): log_warn("drain timeout, discarding " + size); ready_.clear()
    join(workers_); { lock; state_ = Stopped }
```

### 불변조건

- `ready_` 의 `fn` 은 반드시 `try/catch` 로 감싸져 실행. 예외가 워커를 종료시키지 않는다.
- `state_ == Stopped` 일 때 `workers_` 는 모두 join 완료.

---

## 3. 테스트 실행 전략

`InlineExecutor` 는 만들지 않는다. 프로덕션과 의미가 분기되기 때문. 결정적 테스트는 다음으로 수행:

- **JobQueue**: `JobQueue::Pump() / Pump(n) / Flush()` — 호출 스레드에서 `DrainOnce` 를 직접 실행.
- **ThreadPoolExecutor**: 워커 1~2개 + `std::promise` / `std::latch` 동기화.
- **Timer**: `FakeClock` (IClock 구현체) + 대상별 동기화(`JobQueue::Pump` 또는 latch).

---

## 4. `JobQueue` (직렬 실행 큐)

**설계 원칙**:

- 사용자는 `queue.Post(fn)` 만 호출하면 된다. drain 스케줄링, Executor 와의 관계, 직렬성 보장은 내부에서 처리.
- 상태·취소는 `JobEntry` 에 atomic 으로 보관. Handle 은 `weak_ptr<JobEntry>` 로만 접근.
- Cancel 은 lazy: 큐에서 erase 하지 않고 atomic 플립. DrainOnce 에서 skip. 혹은

`server/lib/include/job/job_queue.h`:

```cpp
class JobEntry
{
  public:
    const EntryId                   Id;
    const std::string               DebugLabel;
    const TimePoint                 SubmittedAt;
    IExecutor::Callback             Fn;

    std::atomic<EnumJobState>       State{EnumJobState::Queued};
    std::atomic<bool>               Canceled{false};
};

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
  public:
    using Callback = IExecutor::Callback;

    struct CreateOptions
    {
        uint32_t    QueueCapacity = 0;     // 0 = unbounded
        uint32_t    BatchLimit    = 256;   // 한 drain 에서 연속 실행 최대
        std::string DebugName;
    };

    JobQueue(std::shared_ptr<IExecutor> executor, const CreateOptions& options = {});
    ~JobQueue();
    JobQueue(const JobQueue&)            = delete;
    JobQueue& operator=(const JobQueue&) = delete;

    // 성공 시 유효한 JobHandle, 실패 시 invalid JobHandle.
    JobHandle Post(Callback fn, std::string_view debugLabel = {});

    template <typename Fn> requires std::invocable<Fn>
    JobHandle Post(Fn&& func, std::string_view debugLabel = {})
    { return Post(Callback(std::forward<Fn>(func)), debugLabel); }

    EnumJobQueueState GetState()      const noexcept;
    std::string_view  GetDebugName()  const noexcept;
    uint64_t          GetPendingCount() const noexcept;
    uint32_t          GetBatchLimit() const noexcept;

    void Stop(bool drain = true, Duration drainTimeout = Duration::zero());

    JobQueueStatsSnapshot GetStatsSnapshot() const;   // 유일한 관측 API

    // 테스트 훅
    void Pump();
    void Pump(size_t maxJobs);
    void Flush();

  private:
    void DrainOnce();
};
```

> `enable_shared_from_this` 를 사용하므로 `std::make_shared<JobQueue>(executor, opts)` 로 생성해야 한다. 별도 `Make` / `make_job_queue` 팩토리는 두지 않는다.

### 내부 상태

```cpp
std::shared_ptr<IExecutor>              executor_;
mutable std::mutex                      mtx_;
std::deque<std::shared_ptr<JobEntry>>   queue_;
EntryId                                 nextId_  = 1;
bool                                    running_ = false;   // mtx_ 보호
EnumJobQueueState                       state_   = Running;
CreateOptions                           options_;
std::condition_variable                 stopCv_;
// + stats
```

### 핵심 불변조건

> `running_ == true` ⇔ "Executor 큐에 drain 클로저가 대기 중" 또는 "어떤 워커가 `DrainOnce` 내부에서 실행 중" 중 **정확히 하나**.

- `running_` 과 `queue_` 는 `mtx_` 하에서 함께 관찰/수정.
- 두 워커가 동시에 같은 JobQueue 를 drain 할 수 없다 → 직렬성 보장.

### 동작

```
Post(fn, label):
    entry = make_shared<JobEntry>(nextId_++, label, clock.now(), move(fn))
    need_schedule = false
    { lock(mtx_)
      if state_ != Running:                                   return JobHandle{}
      if QueueCapacity > 0 && queue_.size() >= QueueCapacity:
          stats.rejected++;                                   return JobHandle{}
      queue_.push_back(entry); stats.submitted++
      if !running_: running_ = true; need_schedule = true }
    if need_schedule:
        self = shared_from_this()
        if !executor_->Post([self]{ self->DrainOnce(); }):
            { lock(mtx_); running_=false; state_=Stopped; stopCv_.notify_all() }
            log("queue schedule failed; queue stopped")
            return JobHandle{}
    return JobHandle{ weak(entry), label }

DrainOnce():  # 진입 시 running_ == true 불변
    for i in [0, BatchLimit):
        std::shared_ptr<JobEntry> entry
        { lock(mtx_)
          if queue_.empty(): running_=false; stopCv_.notify_all(); return
          entry = move(queue_.front()); queue_.pop_front() }
        if entry->Canceled.load(acquire):
            entry->State.store(Canceled); continue
        entry->State.store(Executing)
        t0 = clock.now()
        try {
            entry->Fn()
            entry->State.store(Done)
        } catch(...) {
            entry->State.store(Failed); stats.failed++; log
        }
        stats.executed++
        stats.recordExecTime(clock.now() - t0)
        stats.recordWaitTime(t0 - entry->SubmittedAt)

    # BatchLimit 소진, 남은 작업 있음 → self-requeue
    stats.batchLimitHit++
    self = shared_from_this()
    if !executor_->Post([self]{ self->DrainOnce(); }):
        { lock(mtx_); running_=false; state_=Stopped; stopCv_.notify_all() }
        log("queue self-requeue failed")

Stop(drain, timeout):
    { lock(mtx_); state_ = Draining
      if !drain:
          for e in queue_: e->State.store(Canceled)
          queue_.clear() }
    if drain:
        lock(mtx_)
        stopCv_.wait_until(lock, deadline, [] return queue_.empty() && !running_)
        if !queue_.empty() || running_:
            log("queue drain timeout, discarding " + queue_.size())
            for e in queue_: e->State.store(Canceled)
            queue_.clear()
    { lock(mtx_); state_ = Stopped }
```

### 경계 케이스

- **재진입 Post (drain 중 Post)**: `running_==true` 이므로 `need_schedule==false`. 현재 drain 루프 또는 self-requeue 에서 실행.
- **Executor 거절**: 내부 즉시 `state_=Stopped`. 이후 Post 전부 invalid handle.
- **Cancel vs DrainOnce race**: DrainOnce 가 pop 후 `Canceled` 를 atomic 으로 관측. 관측 시점에 따라 0 또는 1회 실행. 실행 완료(Done/Failed) 후 Cancel 은 `AlreadyFinished` 반환.
- **Cancel 된 엔트리가 capacity 계산에 포함**: 대량 cancel 패턴이 없으면 무시 가능.

### 통계 수집 포인트 (`JobQueueStatsSnapshot` 필드)

| 필드                   | 시점                               |
| ---------------------- | ---------------------------------- |
| `SubmittedCount`       | Post 성공                          |
| `RejectedCount`        | Post 실패 (Capacity / Stopped)     |
| `CanceledCount`        | DrainOnce 에서 Canceled 관측       |
| `ExecutedCount`        | DrainOnce fn 호출 직후             |
| `FailedCount`          | fn catch                           |
| `BatchLimitHitCount`   | BatchLimit 도달, self-requeue 직전 |
| `DrainRescheduleCount` | drain 클로저 executor 재등록 성공  |
| `WaitHistogram`        | `SubmittedAt → fn 시작`           |
| `ExecutionHistogram`   | fn 호출 전후                       |
| `Rates`                | 위 카운터를 윈도우 기반으로 파생   |
| `WaitTime` / `ExecutionTime` | 히스토그램에서 P50/P95/P99 파생 |

---

## 5. `Timer`

**설계 원칙**:

- 생성 시 `IExecutor` 고정 바인딩. 사용자는 per-call executor 인자를 넘기지 않는다.
- JobQueue 대상은 별도 overload 로 `shared_ptr<JobQueue>` 를 받아 내부에서 `weak_ptr` 로 저장.
- `TimerEntry` 는 `shared_ptr` 로 bucket 이 소유. Handle 은 `weak_ptr<TimerEntry>`.
- Cancel / Pause / ChangePeriod / ChangeMode 는 Entry atomic 필드만 변경. 이미 dispatch 된 회차에는 영향 없음, 다음 회차부터 반영.
- Watcher 스레드는 사용자 콜백을 직접 실행하지 않는다. 반드시 Executor/JobQueue 로 위임.

`server/lib/include/job/timer.h`:

```cpp
class TimerEntry
{
  public:
    const EntryId       Id;
    const std::string   DebugLabel;

    IExecutor::Callback Fn;
    TimePoint           ScheduledAt;
    uint64_t            Bucket;

    std::variant<std::monostate, std::weak_ptr<JobQueue>> Target;

    std::atomic<EnumJobState> State{EnumJobState::WaitingTimer};
    std::atomic<bool>         Canceled{false};

    // Repeat 전용
    std::optional<Duration>     Period;                           // 설정 여부 표시
    std::atomic<Duration::rep>  PeriodNs{0};                      // 런타임 변경 대상
    std::atomic<EnumRepeatMode> Mode{EnumRepeatMode::FixedDelay};
    std::atomic<bool>           Paused{false};
};

class Timer : public std::enable_shared_from_this<Timer>
{
  public:
    using Callback = IExecutor::Callback;

    struct CreateOptions
    {
        Duration                TickResolution = std::chrono::milliseconds(10);
        std::shared_ptr<IClock> Clock;                 // null → SystemClock
        std::string             DebugName;
    };

    Timer(std::shared_ptr<IExecutor> executor, const CreateOptions& options = {});
    ~Timer();                                          // 내부에서 Stop() 호출 보장
    Timer(const Timer&)            = delete;
    Timer& operator=(const Timer&) = delete;

    void Start();
    void Stop();

    // === 바인딩된 Executor 대상 ===
    JobHandle    ScheduleAt    (TimePoint at, Callback fn, std::string_view label = {});
    JobHandle    ScheduleAfter (Duration delay, Callback fn, std::string_view label = {});
    RepeatHandle ScheduleRepeat(Duration period, Callback fn,
                                EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                                bool executeNow = false,
                                std::string_view label = {});

    // === JobQueue 대상 ===
    JobHandle    ScheduleAt    (std::shared_ptr<JobQueue> target, TimePoint at,
                                Callback fn, std::string_view label = {});
    JobHandle    ScheduleAfter (std::shared_ptr<JobQueue> target, Duration delay,
                                Callback fn, std::string_view label = {});
    RepeatHandle ScheduleRepeat(std::shared_ptr<JobQueue> target, Duration period, Callback fn,
                                EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                                bool executeNow = false,
                                std::string_view label = {});

    size_t   GetPendingCount()   const noexcept;
    Duration GetTickResolution() const noexcept;

  private:
    using BucketKey = uint64_t;

    BucketKey BucketOf(TimePoint tp)   const;
    TimePoint BucketTime(BucketKey b)  const;

    void WatcherLoop();
    bool Dispatch(const std::shared_ptr<TimerEntry>& entry);   // watcher 에서만 호출
    bool Insert(const std::shared_ptr<TimerEntry>& entry);     // mtx_ 하에서 호출, returns was_new_min

    // RepeatHandle::Resume 이 호출하는 private API
    void RequeueEntry(std::shared_ptr<TimerEntry> entry, TimePoint at);

    friend class RepeatHandle;
};
```

> `enable_shared_from_this` 를 사용하므로 `std::make_shared<Timer>(executor, opts)` 로 생성.

### 내부 상태

```cpp
std::shared_ptr<IExecutor>                                            executor_;
std::unordered_map<BucketKey, std::list<std::shared_ptr<TimerEntry>>> buckets_;
std::set<BucketKey>                                                   occupied_;
EntryId                                                               nextId_ = 1;
std::mutex                                                            mtx_;
std::condition_variable                                               cv_;
std::thread                                                           watcher_;
std::atomic<bool>                                                     stopping_{false};
std::shared_ptr<IClock>                                               clock_;
Duration                                                              tickResolution_;
```

### 버킷 모델

```
BucketOf(tp)   = floor(tp.time_since_epoch() / tickResolution_)
BucketTime(b)  = TimePoint{ b * tickResolution_ }
```

- 동일 bucket 내 엔트리는 삽입 순서대로 발화.
- 타이밍 정밀도 = `tickResolution_`.

### 동작

```
ScheduleAt(at, fn, label):
    entry = make_shared<TimerEntry>(nextId_++, label, move(fn), at, BucketOf(at),
                                    Target=monostate{}, ...)
    { lock(mtx_); was_new_min = Insert(entry); if was_new_min: cv_.notify_one() }
    return JobHandle{ weak(entry), label }

ScheduleAt(queue, at, fn, label):         # JobQueue overload
    entry = make_shared<TimerEntry>(... Target=weak_ptr(queue) ...)
    { lock(mtx_); was_new_min = Insert(entry); if was_new_min: cv_.notify_one() }
    return JobHandle{ weak(entry), label }

ScheduleRepeat(period, fn, mode, executeNow, label):
    at = executeNow ? clock_->now() : clock_->now() + period
    entry = make_shared<TimerEntry>(..., Target=monostate{},
                                    Period=period, PeriodNs=period.count(), Mode=mode, ...)
    { lock(mtx_); was_new_min = Insert(entry); if was_new_min: cv_.notify_one() }
    return RepeatHandle{ weak(entry), weak_from_this(), period, mode, label }

Insert(entry):   # mtx_ 보유
    buckets_[entry->Bucket].push_back(entry)
    was_new_min = occupied_.empty() || entry->Bucket < *occupied_.begin()
    occupied_.insert(entry->Bucket)
    return was_new_min

WatcherLoop():
    while !stopping_:
        BucketKey nextBucket
        { lock
          if occupied_.empty():
              cv_.wait_for(lock, long_idle,
                           [] return stopping_ || !occupied_.empty())
              continue
          nextBucket = *occupied_.begin() }
        fireAt = BucketTime(nextBucket)
        if clock_->now() < fireAt:
            { lock; cv_.wait_until(lock, fireAt,
                  [] return stopping_ ||
                           (!occupied_.empty() && *occupied_.begin() < nextBucket)) }
            continue

        std::list<std::shared_ptr<TimerEntry>> fired
        { lock
          if (auto it = buckets_.find(nextBucket); it != buckets_.end()):
              fired.splice(fired.end(), it->second); buckets_.erase(it)
          occupied_.erase(nextBucket) }

        for entry in fired:
            if entry->Canceled.load():
                entry->State.store(Canceled); continue
            if entry->Paused.load():
                continue                                # Resume 시 RequeueEntry 로 재삽입
            bool dispatched = Dispatch(entry)
            if entry->Period && !entry->Canceled.load():
                period = Duration(entry->PeriodNs.load())
                mode   = entry->Mode.load()
                nextAt = ComputeNext(entry->ScheduledAt, mode, period, clock_->now())
                entry->ScheduledAt = nextAt
                entry->Bucket      = BucketOf(nextAt)
                { lock; if Insert(entry): cv_.notify_one() }

Dispatch(entry):
    entry->State.store(Queued)
    visit entry->Target:
      case monostate:
          return executor_->Post(entry->Fn)
      case weak_ptr<JobQueue>:
          if auto q = target.lock():
              return q->Post(entry->Fn, entry->DebugLabel).IsValid()
          entry->State.store(Canceled); log("target queue expired, id=" + Id); return false

ComputeNext(scheduledAt, mode, period, now):
    if mode == FixedDelay:  return now + period                  # 완료 기준
    # FixedRate:
    t = scheduledAt + period
    return (t <= now) ? now : t                                  # catch-up 누적 없음

RequeueEntry(entry, at):
    entry->ScheduledAt = at
    entry->Bucket      = BucketOf(at)
    { lock; if Insert(entry): cv_.notify_one() }

Stop():
    stopping_.store(true); cv_.notify_all()
    if watcher_.joinable(): watcher_.join()
    { lock
      for (b, list) in buckets_:
          for e in list:
              if e->Period: log_debug("repeat canceled on stop, id=" + e->Id)
              e->State.store(Canceled)
      buckets_.clear(); occupied_.clear() }

~Timer():
    Stop()     # 소멸자에서 자동 호출. 이중 호출 안전.
```

### 경계 케이스

- **Cancel vs 발화 race**: Cancel 이 `fired` splice 이전이면 이후 루프에서 `Canceled` 관측 → skip. splice 이후 Dispatch 전이면 역시 `Canceled` 관측 지점에서 skip. splice 이후 Dispatch 직후는 Executor/JobQueue 경로에서 다시 fn 내부의 자기-Cancel 시점이 오고 완료. 결과: **0 또는 1회 실행**.
- **콜백 중 자기 핸들 Cancel**: 현재 회차는 이미 dispatch. 다음 회차 재등록만 취소.
- **ChangePeriod / ChangeMode 타이밍**: 이미 재삽입된 nextAt 에는 반영되지 않고, **그 다음 회차부터** 적용.
- **Pause 된 회차**: 재등록 없이 drop. Resume 시 `RequeueEntry(now + period)` 로 재삽입.
- **Watcher 블로킹 금지**: 사용자 fn 직접 호출 금지. Executor / JobQueue 위임만.

### Handle 경유 조작 (Timer 측 진입점)

```cpp
// 모두 entry 의 atomic 만 건드리고 Timer 컨테이너는 건드리지 않는다.
JobHandle::Cancel():             entry->Canceled.store(true, acq_rel)
RepeatHandle::Pause():           entry->Paused.store(true)
RepeatHandle::Resume():          entry->Paused.store(false);
                                 // 현재 회차가 이미 지나갔으면 Timer 에 재삽입 요청
                                 timer.lock()->RequeueEntry(entry, now + period)
RepeatHandle::ChangePeriod(p):   entry->PeriodNs.store(p.count())
RepeatHandle::ChangeMode(m):     entry->Mode.store(m)
```

---

## 6. `JobHandle` / `RepeatHandle`

**설계 원칙**:

- Entry 가 상태·취소·일시정지·period/mode 를 atomic 으로 보유.
- Handle 은 `weak_ptr<Entry>` + 설정값(label/period/mode) **로컬 캐시**.
- Cancel / Change\* 는 `weak.lock()` 후 atomic 기록 → 컨테이너 인덱스 조회 불필요.
- 핸들 사본 간 로컬 캐시 동기화는 없다. 공유 쓰기가 필요하면 `shared_ptr<RepeatHandle>` 로 공유 소유권을 구성.

`server/lib/include/job/job.h`:

```cpp
class JobHandle
{
    std::variant<std::monostate,
                 std::weak_ptr<JobEntry>,
                 std::weak_ptr<TimerEntry>> ref_;
    std::string debugLabel_;

  public:
    JobHandle() = default;

    bool             IsValid()  const noexcept;          // monostate 아님 && lock() 성공
    bool             Expired()  const noexcept;          // monostate 아님 && lock() 실패
    EnumCancelResult Cancel();                           // atomic 플립 + 현재 상태 리포트
    EnumJobState     TryGetState() const;                // 만료 시 Done 근사
    std::string_view GetDebugLabel() const noexcept { return debugLabel_; }
};

class RepeatHandle
{
    std::weak_ptr<TimerEntry> entry_;
    std::weak_ptr<Timer>      owner_;                    // Resume 재삽입용

    Duration       period_;
    EnumRepeatMode mode_;
    std::string    debugLabel_;

  public:
    RepeatHandle() = default;

    bool IsValid() const noexcept;
    bool Expired() const noexcept;

    bool Cancel();                                       // entry->Canceled = true
    bool Pause();                                        // entry->Paused   = true
    bool Resume();                                       // entry->Paused   = false + Timer::RequeueEntry

    bool ChangePeriod(Duration newPeriod);               // 성공 시 period_ 로컬 캐시 갱신
    bool ChangeMode  (EnumRepeatMode newMode);           // 성공 시 mode_   로컬 캐시 갱신

    Duration         GetPeriod()     const noexcept { return period_; }
    EnumRepeatMode   GetMode()       const noexcept { return mode_; }
    std::string_view GetDebugLabel() const noexcept { return debugLabel_; }
};
```

### 핸들 생성 경로

- `JobQueue::Post(fn, label)` → `JobHandle{ weak<JobEntry>, label }`
- `Timer::ScheduleAt / ScheduleAfter(...)` → `JobHandle{ weak<TimerEntry>, label }`
- `Timer::ScheduleRepeat(...)` → `RepeatHandle{ weak<TimerEntry>, weak<Timer>, period, mode, label }`
- 실패 시 기본(invalid) 핸들: `monostate`, 빈 라벨.

### `JobHandle::Cancel` 상세

```cpp
EnumCancelResult JobHandle::Cancel() {
    auto doCancel = [](auto entry) -> EnumCancelResult {
        if (!entry) return EnumCancelResult::Expired;
        bool was = entry->Canceled.exchange(true, std::memory_order_acq_rel);
        auto s   = entry->State.load();
        if (was) {
            if (s == Done || s == Failed) return EnumCancelResult::AlreadyFinished;
            return EnumCancelResult::AlreadyCanceled;
        }
        if (s == Executing)              return EnumCancelResult::AlreadyExecuting;
        if (s == Done || s == Failed)    return EnumCancelResult::AlreadyFinished;
        return EnumCancelResult::Canceled;
    };
    return std::visit(overloaded {
        [](std::monostate)                            { return EnumCancelResult::Expired; },
        [&](const std::weak_ptr<JobEntry>& w)         { return doCancel(w.lock()); },
        [&](const std::weak_ptr<TimerEntry>& w)       { return doCancel(w.lock()); },
    }, ref_);
}
```

### `RepeatHandle::Resume` 의미

- Paused 가 false 로 바뀌어도 이미 drop 된 회차는 자동 복구되지 않는다.
- Resume 은 반드시 `Timer::RequeueEntry(entry, now + period)` 로 재삽입 요청.
- `entry->Canceled == true` 이면 Resume 은 `false` 반환 + 아무 동작 없음.

### 로컬 캐시 규칙

- `debugLabel` / `period` / `mode` 는 **핸들 인스턴스 단위** 캐시.
- 여러 핸들 사본에서 `ChangePeriod` 를 해도 서로의 `period_` 는 동기화되지 않는다.
- 공유 갱신이 필요하면 `shared_ptr<RepeatHandle>` 으로 소유권을 공유. 단순 읽기는 복사 자유.

---

## 7. 호출 흐름 요약

### (a) `JobQueue::Post` → 실행

```
user → queue.Post(fn, label) → JobHandle{weak<JobEntry>, label}
     ├ queue_.push_back(shared<JobEntry>); stats.submitted++
     └ (first && !running_) → executor_->Post([self]{ self->DrainOnce(); })
executor worker → DrainOnce
     ├ entry->Canceled ? skip; State=Canceled
     ├ fn() 실행 (try/catch) → State=Done/Failed
     ├ 남으면 executor_->Post(drain 재등록)
     └ 비면 running_=false
```

### (b) `Timer → JobQueue`

```
user → timer.ScheduleAfter(queue, delay, fn, label) → JobHandle{weak<TimerEntry>, label}
watcher 만료 → entry->Target.lock() → queue->Post(fn, entry->DebugLabel)
     (target 만료 시 drop + debug log; entry->State=Canceled)
```

### (c) `Timer → Executor` (바인딩)

```
user → timer.ScheduleAfter(delay, fn, label)     # Target = monostate
watcher 만료 → executor_->Post(fn)
worker → fn 실행 (직렬성 없음)
```

### (d) `Executor::Post` (즉시)

```
user → executor.Post(fn) → bool
worker → fn 실행
```

---

## 8. 의존성 · 수명 규칙

- `JobQueue` 는 `shared_ptr<IExecutor>` 를 소유. Executor 가 Queue 보다 오래 살아야 함.
- `Timer` 는 `shared_ptr<IExecutor>` 를 소유 (바인딩). JobQueue target 은 `weak_ptr` 로만 보유 → target 이 먼저 소멸해도 Timer 안전.
- `JobEntry` / `TimerEntry` 는 컨테이너(`queue_` / `buckets_`) 가 `shared_ptr` 로 소유. Handle 은 `weak_ptr`.
  - DrainOnce / fired splice 에서 pop 된 entry 는 스레드 로컬 `shared_ptr` 로 수명 연장 → 실행 중 Cancel 관측 가능.
- `JobQueue::Post` 의 drain 클로저는 `shared_from_this()` 캡쳐 → drain 진행 중 Queue 수명 연장.
- `RepeatHandle::Resume` 은 `weak<Timer>` 를 lock 하여 `RequeueEntry` 호출. Timer 가 소멸했으면 `false` 반환.
- `~Timer()` 는 `Stop()` 을 자동 호출하고 watcher 를 join. 이중 호출 안전.

---

## 9. 빌드 · 파일 조직

```
server/lib/
├── include/job/
│   ├── executor.h                 # IExecutor
│   ├── thread_pool_executor.h
│   ├── job_queue.h                # JobEntry + JobQueue
│   ├── timer.h                    # TimerEntry + Timer
│   ├── job.h                      # JobHandle + RepeatHandle
│   └── job_queue_types.h          # enum / 통계 / EntryId
└── src/job/
    ├── thread_pool_executor.cpp
    ├── job_queue.cpp
    └── timer.cpp
```

- 기존 `job_queue.h` 는 **내용 완전 대체** (파일 경로 재사용).
- `job_queue_helper.h` 제거.
- `inline_executor.*` 는 만들지 않는다.

---

## 10. 구현 진입 전 확정 사항

- `JobQueueStatsSnapshot` / `WorkerPoolStatsSnapshot` 이름 유지. 단일 스냅샷에 `Rates` / `LatencySummary` / `Histogram` 을 모두 포함하여 **관측 API 는 `GetStatsSnapshot()` 하나로 단일화**.
- **Factory 함수 없음**: `ThreadPoolExecutor` / `JobQueue` / `Timer` 모두 public 생성자 + `std::make_shared<T>(...)` 로 생성. `enable_shared_from_this` 사용이므로 `shared_ptr` 생성이 필수라는 점은 문서화로 안내.
- `IClock` 기본 구현 (`SystemClock` 등) 은 **별도 plan 범위**. 본 plan 에서는 `clock.h` 의 `namespace psh;::lib` 오타 수정만 포함.
- `JobHandle::TryGetState()` 가 Entry 소멸 상태에서 호출된 경우: **`Done` 근사 반환** + `Expired()` 로 호출자가 분기 가능.
- `Pause / Resume / ChangePeriod / ChangeMode` 는 atomic 필드 갱신만 수행하며, **이미 dispatch 된 회차에 영향 없음**. 다음 회차부터 반영.
- `Timer::~Timer()` 는 **`Stop()` 자동 호출**. 명시적 `Stop()` 은 테스트/조기 종료 용.
- `RepeatHandle::Resume` 이 이미 `Canceled == true` 인 엔트리에 호출되면 **`false` 반환 + 아무 동작 없음**.
