#pragma once

#include "executor.h"
#include "job.h"
#include "job_entry.h"

#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

namespace psh::lib::job
{
// ------------------------------------------------------------
// JobQueue — IExecutor 위에 쌓이는 직렬 실행 큐.
//
// 생성 규약:
//   std::make_shared<JobQueue>(executor, options)
//   enable_shared_from_this 사용. 반드시 shared_ptr 로 생성.
//
// 핵심 불변:
//   running_ == true  ⇔  "executor 큐에 drain 클로저가 대기 중" 또는
//                        "어떤 워커가 DrainOnce 내부에서 실행 중" (둘 중 하나).
//   running_ 과 queue_ 는 mtx_ 하에서 함께 관찰/수정.
// ------------------------------------------------------------

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
  public:
    using Callback = IExecutor::Callback;

    struct CreateOptions
    {
        uint32_t QueueCapacity = 0;  // 0 = unbounded
        uint32_t BatchLimit = 256;   // 한 drain 에서 연속 실행 최대
        std::string DebugName;
    };

    JobQueue(std::shared_ptr<IExecutor> executor, CreateOptions options = {})
        : executor_(std::move(executor)), options_(std::move(options))
    {
    }

    ~JobQueue()
    {
        Stop(false, Duration::zero());
    }

    JobQueue(const JobQueue&) = delete;
    JobQueue& operator=(const JobQueue&) = delete;

  public:
    // 성공 시 유효한 JobHandle, 실패 시 invalid JobHandle.
    JobHandle Post(Callback fn, std::string_view debugLabel = {})
    {
        if (!fn)
            return JobHandle{};

        auto entry = std::make_shared<JobEntry>(
            NextId(), debugLabel, std::chrono::steady_clock::now(), std::move(fn));

        bool needSchedule = false;
        {
            std::lock_guard lock(mtx_);
            if (state_ != EnumJobQueueState::Running)
                return JobHandle{};
            if (options_.QueueCapacity > 0
                && queue_.size() >= options_.QueueCapacity)
            {
                rejectedCount_.fetch_add(1, std::memory_order_relaxed);
                return JobHandle{};
            }
            queue_.push_back(entry);
            submittedCount_.fetch_add(1, std::memory_order_relaxed);
            if (!running_)
            {
                running_ = true;
                needSchedule = true;
            }
        }

        if (needSchedule)
        {
            auto self = shared_from_this();
            if (!executor_->Post([self] { self->DrainOnce(); }))
            {
                MarkFailedSchedule();
                return JobHandle{};
            }
        }

        return JobHandle{std::weak_ptr<JobEntry>(entry), debugLabel};
    }

    template <typename Fn>
        requires std::invocable<Fn>
    JobHandle Post(Fn&& func, std::string_view debugLabel = {})
    {
        return Post(Callback(std::forward<Fn>(func)), debugLabel);
    }

  public:
    [[nodiscard]] EnumJobQueueState GetState() const noexcept
    {
        std::lock_guard lock(mtx_);
        return state_;
    }

    [[nodiscard]] std::string_view GetDebugName() const noexcept
    {
        return options_.DebugName;
    }

    [[nodiscard]] uint64_t GetPendingCount() const noexcept
    {
        std::lock_guard lock(mtx_);
        return static_cast<uint64_t>(queue_.size());
    }

    [[nodiscard]] uint32_t GetBatchLimit() const noexcept
    {
        return options_.BatchLimit;
    }

    void Stop(bool drain = true, Duration drainTimeout = Duration::zero())
    {
        {
            std::lock_guard lock(mtx_);
            if (state_ == EnumJobQueueState::Stopped)
                return;
            state_ = EnumJobQueueState::Draining;
            if (!drain)
            {
                for (auto& e : queue_)
                    e->State.store(EnumJobState::Canceled,
                                   std::memory_order_release);
                canceledCount_.fetch_add(queue_.size(),
                                         std::memory_order_relaxed);
                queue_.clear();
            }
        }

        if (drain)
        {
            std::unique_lock lock(mtx_);
            auto pred = [&] { return queue_.empty() && !running_; };

            if (drainTimeout == Duration::zero())
            {
                stopCv_.wait(lock, pred);
            }
            else
            {
                const auto deadline =
                    std::chrono::steady_clock::now() + drainTimeout;
                if (!stopCv_.wait_until(lock, deadline, pred))
                {
                    const auto dropped = queue_.size();
                    for (auto& e : queue_)
                        e->State.store(EnumJobState::Canceled,
                                       std::memory_order_release);
                    canceledCount_.fetch_add(dropped,
                                             std::memory_order_relaxed);
                    queue_.clear();
                }
            }
        }

        {
            std::lock_guard lock(mtx_);
            state_ = EnumJobQueueState::Stopped;
        }
    }

  public:
    [[nodiscard]] JobQueueStatsSnapshot GetStatsSnapshot() const
    {
        JobQueueStatsSnapshot snap;
        {
            std::lock_guard lock(mtx_);
            snap.State = state_;
            snap.PendingCount = static_cast<uint64_t>(queue_.size());
        }
        snap.SubmittedCount = submittedCount_.load(std::memory_order_relaxed);
        snap.ExecutedCount = executedCount_.load(std::memory_order_relaxed);
        snap.FailedCount = failedCount_.load(std::memory_order_relaxed);
        snap.RejectedCount = rejectedCount_.load(std::memory_order_relaxed);
        snap.CanceledCount = canceledCount_.load(std::memory_order_relaxed);
        snap.BatchLimitHitCount =
            batchLimitHitCount_.load(std::memory_order_relaxed);
        snap.DrainRescheduleCount =
            drainRescheduleCount_.load(std::memory_order_relaxed);
        return snap;
    }

  public:
    // 테스트 훅: 호출 스레드에서 DrainOnce 를 직접 실행.
    void Pump() { Pump(static_cast<size_t>(options_.BatchLimit)); }

    void Pump(size_t maxJobs)
    {
        for (size_t i = 0; i < maxJobs; ++i)
        {
            std::shared_ptr<JobEntry> entry;
            {
                std::lock_guard lock(mtx_);
                if (queue_.empty())
                    return;
                entry = std::move(queue_.front());
                queue_.pop_front();
            }
            ExecuteEntry(entry);
        }
    }

    void Flush()
    {
        while (true)
        {
            std::shared_ptr<JobEntry> entry;
            {
                std::lock_guard lock(mtx_);
                if (queue_.empty())
                    return;
                entry = std::move(queue_.front());
                queue_.pop_front();
            }
            ExecuteEntry(entry);
        }
    }

  private:
    EntryId NextId() noexcept
    {
        return nextId_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    void MarkFailedSchedule()
    {
        std::unique_lock lock(mtx_);
        running_ = false;
        state_ = EnumJobQueueState::Stopped;
        stopCv_.notify_all();
    }

    void ExecuteEntry(const std::shared_ptr<JobEntry>& entry)
    {
        if (entry->Canceled.load(std::memory_order_acquire))
        {
            entry->State.store(EnumJobState::Canceled,
                               std::memory_order_release);
            canceledCount_.fetch_add(1, std::memory_order_relaxed);
            return;
        }
        entry->State.store(EnumJobState::Executing,
                           std::memory_order_release);
        try
        {
            entry->Fn();
            entry->State.store(EnumJobState::Done,
                               std::memory_order_release);
        }
        catch (...)
        {
            entry->State.store(EnumJobState::Failed,
                               std::memory_order_release);
            failedCount_.fetch_add(1, std::memory_order_relaxed);
            // TODO: logger 연결 시 예외 메시지 기록.
        }
        executedCount_.fetch_add(1, std::memory_order_relaxed);
    }

    void DrainOnce()
    {
        // 진입 시 running_ == true 불변.
        for (uint32_t i = 0; i < options_.BatchLimit; ++i)
        {
            std::shared_ptr<JobEntry> entry;
            {
                std::lock_guard lock(mtx_);
                if (queue_.empty())
                {
                    running_ = false;
                    stopCv_.notify_all();
                    return;
                }
                entry = std::move(queue_.front());
                queue_.pop_front();
            }
            ExecuteEntry(entry);
        }

        // BatchLimit 소진, 남은 작업 있음 → self-requeue.
        batchLimitHitCount_.fetch_add(1, std::memory_order_relaxed);
        auto self = shared_from_this();
        if (executor_->Post([self] { self->DrainOnce(); }))
        {
            drainRescheduleCount_.fetch_add(1, std::memory_order_relaxed);
        }
        else
        {
            MarkFailedSchedule();
        }
    }

  private:
    std::shared_ptr<IExecutor> executor_;
    CreateOptions options_;

    mutable std::mutex mtx_;
    std::condition_variable stopCv_;
    std::deque<std::shared_ptr<JobEntry>> queue_;

    std::atomic<EntryId> nextId_{0};
    bool running_ = false;
    EnumJobQueueState state_ = EnumJobQueueState::Running;

    std::atomic<uint64_t> submittedCount_{0};
    std::atomic<uint64_t> executedCount_{0};
    std::atomic<uint64_t> failedCount_{0};
    std::atomic<uint64_t> rejectedCount_{0};
    std::atomic<uint64_t> canceledCount_{0};
    std::atomic<uint64_t> batchLimitHitCount_{0};
    std::atomic<uint64_t> drainRescheduleCount_{0};
};

} // namespace psh::lib::job
