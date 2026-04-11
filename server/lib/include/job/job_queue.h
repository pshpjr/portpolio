#pragma once

#include "job_queue_types.h"

#include <functional>
#include <memory>
#include <string_view>

namespace psh::lib::job
{
class WorkerPool;

// ------------------------------------------------------------
// JobQueue
// ------------------------------------------------------------
// 외부 사용자는 Job 클래스를 알 필요 없다.
// callable(std::function<void()> 또는 호출 가능 객체)을 직접 전달한다.

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
  public:
    using Callback = std::function<void()>;

    struct CreateOptions
    {
        std::shared_ptr<WorkerPool> Executor;

        uint32_t QueueCapacity = 0; // 0 = unbounded
        uint32_t BatchLimit = 256;

        SlowJobOptions SlowJob{};
        std::string DebugName;
    };

  public:
    explicit JobQueue(const CreateOptions& options);
    ~JobQueue();

    JobQueue(const JobQueue&) = delete;
    JobQueue& operator=(const JobQueue&) = delete;

  public:
    EnumJobQueueState GetState() const noexcept;
    std::string_view GetDebugName() const noexcept;

    void Stop(bool drain = true, Duration drainTimeout = Duration::zero());

  public:
    bool Post(Callback callback);
    bool Push(Callback callback);

    template <typename Fn>
        requires std::invocable<Fn>
    bool Post(Fn&& func)
    {
        return Post(Callback(std::forward<Fn>(func)));
    }

    template <typename Fn>
        requires std::invocable<Fn>
    bool Push(Fn&& func)
    {
        return Push(Callback(std::forward<Fn>(func)));
    }

  public:
    JobQueueStatsSnapshot GetStatsSnapshot() const;
    RateStats GetRateStats() const;
    HistogramSnapshot GetWaitHistogram() const;
    HistogramSnapshot GetExecutionTimeHistogram() const;

    uint64_t GetPendingCount() const noexcept;
    uint64_t GetTimedWaitingCount() const noexcept;
    uint32_t GetBatchLimit() const noexcept;

  public:
    // test / debug support
    void Pump();
    void Pump(size_t maxJobs);
    void Flush();
};

} // namespace psh::lib::job
