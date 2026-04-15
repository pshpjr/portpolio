#pragma once

#include "entry.h"
#include "executor.h"
#include "job_handle.h"

#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace psh::lib::job
{

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
    using Callback = IExecutor::Callback;

    struct CreateOptions
    {
        uint32_t BatchLimit = 256;
        std::string DebugName;
    };

    JobQueue(std::shared_ptr<IExecutor> executor, CreateOptions options = {})
        : executor_(std::move(executor)),
          options_(std::move(options)),
          waitHistogram_(DefaultLatencyBoundsNs()),
          execHistogram_(DefaultLatencyBoundsNs())
    {
        if (!executor_)
            throw std::invalid_argument("JobQueue requires a non-null executor");
    }

    ~JobQueue()
    {
        Stop(false, Duration::zero());
    }

    JobQueue(const JobQueue&) = delete;
    JobQueue& operator=(const JobQueue&) = delete;

public:
    JobHandle Post(Callback fn, std::string_view debugLabel = {})
    {
        if (!fn)
            return {};

        auto entry = std::make_shared<Entry>(NextId(), debugLabel, std::move(fn), std::chrono::steady_clock::now());

        bool needSchedule = false;
        {
            std::lock_guard lock(mtx_);

            if (state_ != EnumJobQueueState::Running)
                return {};

            queue_.push_back(entry);

            //  내가 true로 바꾼 경우
            if (!std::exchange(running_, true))
                needSchedule = true;
        }

        submittedCount_.fetch_add(1, std::memory_order_relaxed);
        submittedRate_.Add(1);

        if (needSchedule)
        {
            executor_->Post([this, self = shared_from_this()]
                            {
                                DrainOnce();
                            });
        }

        return JobHandle{std::weak_ptr(entry), debugLabel};
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
        return queue_.size();
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
                for (auto& entry : queue_)
                    entry->state_.store(EnumJobState::Canceled, std::memory_order_release);
                canceledCount_.fetch_add(queue_.size(), std::memory_order_relaxed);
                queue_.clear();
            }
        }
        stopCv_.notify_all();

        if (drain)
        {
            std::unique_lock lock(mtx_);
            auto pred = [&]
            {
                return queue_.empty() && !running_;
            };

            if (drainTimeout == Duration::zero())
            {
                stopCv_.wait(lock, pred);
            }
            else if (const auto deadline = std::chrono::steady_clock::now() + drainTimeout;
                     !stopCv_.wait_until(lock, deadline, pred))
            {
                for (auto& entry : queue_)
                    entry->state_.store(EnumJobState::Canceled, std::memory_order_release);
                canceledCount_.fetch_add(queue_.size(), std::memory_order_relaxed);
                queue_.clear();
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
            snap.PendingCount = queue_.size();
        }
        snap.SubmittedCount = submittedCount_.load(std::memory_order_relaxed);
        snap.ExecutedCount = executedCount_.load(std::memory_order_relaxed);
        snap.FailedCount = failedCount_.load(std::memory_order_relaxed);
        snap.CanceledCount = canceledCount_.load(std::memory_order_relaxed);
        snap.BatchLimitHitCount = batchLimitHitCount_.load(std::memory_order_relaxed);
        snap.DrainRescheduleCount = drainRescheduleCount_.load(std::memory_order_relaxed);

        snap.SubmittedRate = submittedRate_.Snapshot();
        snap.ExecutedRate = executedRate_.Snapshot();
        snap.FailedRate = failedRate_.Snapshot();

        snap.WaitHistogram = waitHistogram_.Snapshot();
        snap.ExecutionHistogram = execHistogram_.Snapshot();
        snap.WaitTime = telemetry::Summarize(snap.WaitHistogram);
        snap.ExecutionTime = telemetry::Summarize(snap.ExecutionHistogram);
        return snap;
    }

#if defined(PSH_JOBQUEUE_ENABLE_TEST_API)
public:
    void Pump()
    {
        Pump(static_cast<size_t>(options_.BatchLimit));
    }

    void Pump(size_t maxJobs)
    {
        for (size_t i = 0; i < maxJobs; ++i)
        {
            std::shared_ptr<Entry> entry;
            {
                std::lock_guard lock(mtx_);
                if (running_ || queue_.empty())
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
            std::shared_ptr<Entry> entry;
            {
                std::lock_guard lock(mtx_);
                if (running_ || queue_.empty())
                    return;
                entry = std::move(queue_.front());
                queue_.pop_front();
            }
            ExecuteEntry(entry);
        }
    }
#endif

private:
    EntryId NextId() noexcept
    {
        return nextId_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    void ExecuteEntry(const std::shared_ptr<Entry>& entry)
    {
        using namespace std::chrono;

        if (entry->cancelRequested_.load(std::memory_order_acquire))
        {
            entry->state_.store(EnumJobState::Canceled, std::memory_order_release);
            canceledCount_.fetch_add(1, std::memory_order_relaxed);
            return;
        }

        const auto startedAt = steady_clock::now();
        waitHistogram_.Record(duration_cast<nanoseconds>(startedAt - entry->scheduledAt_).count());

        entry->state_.store(EnumJobState::Executing, std::memory_order_release);
        try
        {
            entry->fn_();
            entry->state_.store(EnumJobState::Done, std::memory_order_release);
        }
        catch (...)
        {
            entry->state_.store(EnumJobState::Failed, std::memory_order_release);
            failedCount_.fetch_add(1, std::memory_order_relaxed);
            failedRate_.Add(1);
            return;
        }

        execHistogram_.Record(duration_cast<nanoseconds>(steady_clock::now() - startedAt).count());

        executedCount_.fetch_add(1, std::memory_order_relaxed);
        executedRate_.Add(1);
    }

    void DrainOnce()
    {
        uint32_t processed = 0;
        std::vector<std::shared_ptr<Entry>> entries;
        entries.reserve(options_.BatchLimit);

        while (processed < options_.BatchLimit)
        {
            entries.clear();
            const size_t remainBudget = options_.BatchLimit - processed;

            {
                std::lock_guard lock(mtx_);

                const size_t count = std::min(queue_.size(), remainBudget);

                if (count == 0)
                {
                    running_ = false;
                    stopCv_.notify_all();
                    return;
                }

                for (size_t i = 0; i < count; ++i)
                {
                    entries.emplace_back(std::move(queue_.front()));
                    queue_.pop_front();
                }
            }

            for (auto& entry : entries)
            {
                ExecuteEntry(entry);
            }

            processed += static_cast<uint32_t>(entries.size());
        }

        bool shouldReschedule = false;
        {
            std::lock_guard lock(mtx_);
            if (!queue_.empty())
            {
                shouldReschedule = true;
            }
            else
            {
                running_ = false;
                stopCv_.notify_all();
            }
        }

        if (shouldReschedule)
        {
            batchLimitHitCount_.fetch_add(1, std::memory_order_relaxed);
            drainRescheduleCount_.fetch_add(1, std::memory_order_relaxed);

            executor_->Post([this, self = shared_from_this()]()
                            {
                                DrainOnce();
                            });
        }
    }

private:
    std::shared_ptr<IExecutor> executor_;
    CreateOptions options_;

    mutable std::mutex mtx_;
    std::condition_variable stopCv_;
    std::deque<std::shared_ptr<Entry>> queue_;

    std::atomic<EntryId> nextId_{};
    bool running_ = false;
    EnumJobQueueState state_ = EnumJobQueueState::Running;

    std::atomic<uint64_t> submittedCount_{};
    std::atomic<uint64_t> executedCount_{};
    std::atomic<uint64_t> failedCount_{};
    std::atomic<uint64_t> canceledCount_{};
    std::atomic<uint64_t> batchLimitHitCount_{};
    std::atomic<uint64_t> drainRescheduleCount_{};

    // 계측 (psh::lib::telemetry — 도메인 무관 Recorder 재사용).
    telemetry::RateMeter submittedRate_;
    telemetry::RateMeter executedRate_;
    telemetry::RateMeter failedRate_;
    telemetry::Histogram waitHistogram_;
    telemetry::Histogram execHistogram_;

    // ns 단위 기본 버킷. job 호출 빈도와 지연 분포(수 µs ~ 수십 s)를 가정.
    static std::vector<uint64_t> DefaultLatencyBoundsNs()
    {
        return {
            1'000ULL,          // 1us
            10'000ULL,         // 10us
            100'000ULL,        // 100us
            1'000'000ULL,      // 1ms
            10'000'000ULL,     // 10ms
            100'000'000ULL,    // 100ms
            1'000'000'000ULL,  // 1s
            10'000'000'000ULL, // 10s
        };
    }
};

} // namespace psh::lib::job
