# Job System Interface Refactoring Log

## Before (초기 버전)

### 파일 구성
- `job_queue_types.h`, `job.h`, `job_queue.h`, `worker_pool.h`, `timed_job_manager.h`

### job_queue_types.h
```cpp
// 타입 별칭
using TimePoint = std::chrono::steady_clock::time_point;
using Duration  = std::chrono::nanoseconds;

// Enums
enum class JobState        { WaitingTimer, Queued, Executing, Done, Canceled, Rejected };
enum class JobQueueState   { Running, Draining, Stopped };
enum class RepeatMode      { FixedRate, FixedDelay };
enum class StopMode        { RejectNewAndDrain, RejectNewAndDiscardPending };
enum class SubmitMode      { Post, Push };
enum class BindMode        { Strong, Weak };
enum class CancelResult    { Canceled, AlreadyExecuting, AlreadyFinished, NotFound, Rejected };

// Option structs
struct StopOptions     { StopMode Mode; Duration DrainTimeout; };
struct SlowJobOptions  { Duration WarningThreshold; bool EnableWarningLog; };
struct SubmitOptions   { SubmitMode Mode; optional<TimePoint> ExecuteAt; optional<Duration> ExecuteAfter; string DebugLabel; };
struct RepeatOptions   { Duration Period; bool ExecuteNow; RepeatMode Mode; string DebugLabel; };

// 통계 구조체들 (HistogramBucket, HistogramSnapshot, RateStats, LatencySummary, ...)

// 추상 인터페이스
class IExecutor {
    virtual bool Post(Job job) = 0;
    virtual bool Push(Job job) = 0;
    virtual bool PostAt(TimePoint at, Job job) = 0;
    virtual bool PushAt(TimePoint at, Job job) = 0;
    virtual bool PostAfter(Duration delay, Job job) = 0;
    virtual bool PushAfter(Duration delay, Job job) = 0;
};
```

### job.h
```cpp
class Job {
    using Callback = std::function<void()>;

    Job() = default;
    explicit Job(Callback callback, string_view debugLabel = {});
    template <typename F> explicit Job(F&& func, string_view debugLabel = {});
    // shared_ptr 멤버 함수 바인딩 생성자 (const / non-const)

    bool IsValid() const noexcept;
    string_view GetDebugLabel() const noexcept;
};

class JobHandle { ... };   // Cancel, TryGetState
class RepeatHandle { ... }; // Stop, Pause, Resume, ChangePeriod, ChangeMode
```

### worker_pool.h
```cpp
class WorkerPool : public IExecutor, public enable_shared_from_this<WorkerPool> {
    struct CreateOptions { uint32_t WorkerCount; uint32_t QueueCapacity; SlowJobOptions SlowJob; };

    bool Start();
    void Stop(const StopOptions& options);
    JobQueueState GetState() const noexcept;

    // IExecutor overrides (6개)
    bool Post(Job job) override;
    bool Push(Job job) override;
    bool PostAt(TimePoint at, Job job) override;
    bool PushAt(TimePoint at, Job job) override;
    bool PostAfter(Duration delay, Job job) override;
    bool PushAfter(Duration delay, Job job) override;

    // 템플릿 버전 (6개)
    template <typename F> bool Post(F&&, string_view = {});
    template <typename F> bool Push(F&&, string_view = {});
    template <typename F> bool PostAt(TimePoint, F&&, string_view = {});
    template <typename F> bool PushAt(TimePoint, F&&, string_view = {});
    template <typename F> bool PostAfter(Duration, F&&, string_view = {});
    template <typename F> bool PushAfter(Duration, F&&, string_view = {});

    // 통계
    WorkerPoolStatsSnapshot GetStatsSnapshot() const;
    RateStats GetRateStats() const;
    HistogramSnapshot GetQueueWaitHistogram() const;
    HistogramSnapshot GetExecutionTimeHistogram() const;
    vector<WorkerStateSnapshot> GetWorkerStates() const;
};
```

### job_queue.h
```cpp
class JobQueue : public enable_shared_from_this<JobQueue> {
    struct CreateOptions {
        shared_ptr<WorkerPool> Executor;
        shared_ptr<TimedJobManager> TimerManager;
        uint32_t QueueCapacity; uint32_t BatchLimit;
        SlowJobOptions SlowJob; string DebugName;
    };

    void Stop(const StopOptions& options);

    // Post/Push (Job 직접 + 시간 기반 6종)
    bool Post(Job job);       bool Push(Job job);
    bool PostAt(TimePoint, Job);  bool PushAt(TimePoint, Job);
    bool PostAfter(Duration, Job) override;  bool PushAfter(Duration, Job) override;

    // 템플릿 (6종 + Repeat + Submit)
    template <typename F> bool Post(F&&, string_view = {});
    template <typename F> bool Push(F&&, string_view = {});
    template <typename F> bool PostAt(TimePoint, F&&, string_view = {});
    template <typename F> bool PushAt(TimePoint, F&&, string_view = {});
    template <typename F> bool PostAfter(Duration, F&&, string_view = {});
    template <typename F> bool PushAfter(Duration, F&&, string_view = {});
    template <typename F> RepeatHandle Repeat(const RepeatOptions&, F&&);

    JobHandle Submit(Job, const SubmitOptions&);
    template <typename F> JobHandle Submit(F&&, const SubmitOptions&);

    // 통계 + 테스트
    void Pump(); void Pump(size_t); void Flush();
};
```

### timed_job_manager.h
```cpp
class TimedJobManager : public enable_shared_from_this<TimedJobManager> {
    struct CreateOptions { shared_ptr<IClock> Clock; SlowJobOptions SlowJob; };

    bool Start();
    void Stop(const StopOptions& options);

    // IExecutor 대상 (5종)
    JobHandle PostAt(shared_ptr<IExecutor>, TimePoint, Job);
    JobHandle PushAt(shared_ptr<IExecutor>, TimePoint, Job);
    JobHandle PostAfter(shared_ptr<IExecutor>, Duration, Job);
    JobHandle PushAfter(shared_ptr<IExecutor>, Duration, Job);
    RepeatHandle Repeat(shared_ptr<IExecutor>, const RepeatOptions&, Job);

    // JobQueue 대상 (5종)
    JobHandle PostAt(shared_ptr<JobQueue>, TimePoint, Job);
    JobHandle PushAt(shared_ptr<JobQueue>, TimePoint, Job);
    JobHandle PostAfter(shared_ptr<JobQueue>, Duration, Job);
    JobHandle PushAfter(shared_ptr<JobQueue>, Duration, Job);
    RepeatHandle Repeat(shared_ptr<JobQueue>, const RepeatOptions&, Job);

    // 템플릿 (10종)
    // ... IExecutor/JobQueue x PostAt/PushAt/PostAfter/PushAfter/Repeat

    // 테스트
    void FlushDueJobs();
    void AdvanceForTest(Duration);
    void AdvanceToForTest(TimePoint);
};
```

---

## After (현재 버전)

### 설계 원칙
1. **책임 분리**: 시간 관련 로직은 WorkerPool/JobQueue에서 분리
2. **Job 인자 우선**: Job은 항상 첫 번째 인자
3. **Push + 시간 = 모순**: Push(즉시 실행 가능)와 After/At(지연)은 의미가 맞지 않으므로 시간 기반은 Post만
4. **JobQueue 사용자는 Job을 모름**: callable을 직접 전달
5. **Option struct 최소화**: StopOptions, SubmitOptions, RepeatOptions 제거, 인자 직접 전달
6. **IExecutor 제거**: 불필요한 추상화
7. **Repeat은 자기 재등록**: TimedJobManager가 Repeat을 관리하지 않고, Job이 실행 후 자기를 다시 PostAt으로 등록
8. **TimedJobManager는 대상을 모른다**: PostAt(Job, TimePoint)만 제공. 대상(JobQueue/WorkerPool)은 Job 안에 캡처된다
9. **RepeatHandle: Stop/Cancel 합침**: Cancel 하나로 통합. 미래 회차 + 이미 예약된 회차 모두 취소

### 파일 구성
- `job_queue_types.h`, `job.h`, `job_queue.h`, `worker_pool.h`, `timed_job_manager.h`, **`job_queue_helper.h`** (신규)

### job_queue_types.h
```cpp
// Enums (정리됨)
enum class EnumJobState       { WaitingTimer, Queued, Executing, Done, Canceled, Rejected };
enum class EnumJobQueueState  { Running, Draining, Stopped };
enum class EnumRepeatMode     { FixedRate, FixedDelay };
enum class EnumCancelResult   { Canceled, AlreadyExecuting, AlreadyFinished, NotFound, Rejected };

// 삭제: StopMode, SubmitMode, BindMode
// 삭제: IExecutor, StopOptions, SubmitOptions, RepeatOptions

struct SlowJobOptions { ... };
// 통계 구조체들 유지
```

### job.h
```cpp
class Job {
    using Callback = std::function<void()>;

    Job() = default;
    Job(Callback callback, string_view debugLabel = {});  // 암시적 변환 허용

    template <typename Fn>
        requires std::invocable<Fn> && (!std::same_as<decay_t<Fn>, Job>)
    Job(Fn&& func, string_view debugLabel = {});  // 암시적 변환 허용

    // shared_ptr 멤버 함수 바인딩 (const / non-const)

    [[nodiscard]] bool IsValid() const noexcept;
    [[nodiscard]] string_view GetDebugLabel() const noexcept;
    void operator()() const;

  private:
    Callback callback_;
    std::string debugLabel_;  // string_view -> string으로 변경
};

class JobHandle { ... };    // Cancel -> EnumCancelResult, TryGetState -> EnumJobState
class RepeatHandle { ... }; // EnumRepeatMode 사용
```

### worker_pool.h
```cpp
class WorkerPool : public enable_shared_from_this<WorkerPool> {  // IExecutor 상속 제거
    struct CreateOptions { uint32_t WorkerCount; uint32_t QueueCapacity; SlowJobOptions SlowJob; };

    bool Start();
    void Stop(bool drain = true, Duration drainTimeout = Duration::zero());  // 인자 직접 전달
    EnumJobQueueState GetState() const noexcept;

    // Post/Push만 (시간 관련 제거)
    bool Post(Job job);
    bool Push(Job job);
    template <typename F> bool Post(F&&, string_view = {});
    template <typename F> bool Push(F&&, string_view = {});

    // 통계 (유지)
};
```

### job_queue.h
```cpp
class JobQueue : public enable_shared_from_this<JobQueue> {
    using Callback = std::function<void()>;

    struct CreateOptions {
        shared_ptr<WorkerPool> Executor;
        // TimerManager 제거
        uint32_t QueueCapacity; uint32_t BatchLimit;
        SlowJobOptions SlowJob; string DebugName;
    };

    void Stop(bool drain = true, Duration drainTimeout = Duration::zero());

    // Job 클래스를 노출하지 않음. callable 직접 전달.
    bool Post(Callback callback);
    bool Push(Callback callback);
    template <typename Fn> requires std::invocable<Fn> bool Post(Fn&&);
    template <typename Fn> requires std::invocable<Fn> bool Push(Fn&&);

    // 삭제: PostAt, PushAt, PostAfter, PushAfter, Repeat, Submit
    // 통계 + 테스트 (Pump, Flush 유지)
};
```

### timed_job_manager.h
```cpp
class TimedJobManager : public enable_shared_from_this<TimedJobManager> {
    struct CreateOptions { shared_ptr<IClock> Clock; SlowJobOptions SlowJob; };

    bool Start();
    void Stop(bool drain = true, Duration drainTimeout = Duration::zero());
    EnumJobQueueState GetState() const noexcept;

    // 핵심 단일 인터페이스: 지정 시점에 Job을 실행한다.
    // 대상(JobQueue/WorkerPool)은 Job 안에 캡처되어 있다.
    JobHandle PostAt(Job job, TimePoint at);

    uint64_t GetWaitingCount() const noexcept;
    void FlushDueJobs();  // 테스트용

    // 삭제: shared_ptr<JobQueue>/IExecutor/WorkerPool 대상 파라미터
    // 삭제: PushAt, PostAfter, PushAfter, Repeat (모두)
    // 삭제: AdvanceForTest, AdvanceToForTest (시간 제어는 IClock 책임)
};
```

### job_queue_helper.h (신규)
```cpp
struct JobQueueHelper {
    // 시간 기반 제출 (TimedJobManager 경유).
    // job을 대상->Post로 감싸서 TimedJobManager::PostAt(wrappedJob, at)에 위임.
    // JobQueue 대상과 WorkerPool 대상 overload를 모두 제공.

    // JobQueue 대상
    static bool PostAt(Job, shared_ptr<JobQueue>, shared_ptr<TimedJobManager>, TimePoint);
    static bool PostAfter(Job, shared_ptr<JobQueue>, shared_ptr<TimedJobManager>, Duration);
    static RepeatHandle Repeat(Job, shared_ptr<JobQueue>, shared_ptr<TimedJobManager>,
                               Duration period, EnumRepeatMode = FixedDelay, bool executeNow = false);

    // WorkerPool 대상
    static bool PostAt(Job, shared_ptr<WorkerPool>, shared_ptr<TimedJobManager>, TimePoint);
    static bool PostAfter(Job, shared_ptr<WorkerPool>, shared_ptr<TimedJobManager>, Duration);
    static RepeatHandle Repeat(Job, shared_ptr<WorkerPool>, shared_ptr<TimedJobManager>,
                               Duration period, EnumRepeatMode = FixedDelay, bool executeNow = false);

    // 각각 템플릿 버전 제공

    // Repeat 계약:
    //   FixedDelay — 실행 완료 시점 + period
    //   FixedRate  — 이전 예약 시점 + period. 지연 누적 시 즉시 1회 (catch-up 없음)
    //   Cancel     — 미래 + 예약된 회차 모두 취소. 콜백 내 자기 Cancel 허용
    //   Pause      — 예약 취소 후 대기
    //   Resume     — period 리셋 후 재시작
};
```

---

## 삭제된 항목 요약

| 항목 | 이유 |
|------|------|
| `IExecutor` | 불필요한 추상화. WorkerPool 직접 사용 |
| `StopMode`, `StopOptions` | `bool drain, Duration drainTimeout`로 대체 |
| `SubmitMode`, `SubmitOptions` | Post/Push 직접 호출로 대체 |
| `BindMode` | 미사용 |
| `RepeatOptions` | 인자 직접 전달로 대체 |
| `Push + 시간 기반` (PushAt, PushAfter) | Push(즉시 실행 가능)와 지연은 의미 모순 |
| `TimedJobManager::Repeat` | Job이 자기 자신을 재등록하는 방식으로 전환 |
| `TimedJobManager::AdvanceForTest/AdvanceToForTest` | 시간 제어는 IClock 책임 |
| `TimedJobManager`의 대상 파라미터 (`shared_ptr<JobQueue>` 등) | Job 안에 대상 캡처. PostAt(Job, TimePoint)만 남김 |
| `RepeatHandle::Stop` | Cancel로 통합 (미래 + 예약분 모두 취소) |
| `JobQueue` 내 시간 메서드 전체 | JobQueueHelper로 이동 |
| `JobQueue`의 Job 노출 | callable 직접 전달로 은닉 |
