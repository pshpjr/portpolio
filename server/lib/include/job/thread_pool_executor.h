#pragma once

#include "executor.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace psh::lib::job
{
// ------------------------------------------------------------
// ThreadPoolExecutor — N 워커로 Callback 을 소비한다.
//
// 생성 규약:
//   std::make_shared<ThreadPoolExecutor>(options)
//   (enable_shared_from_this 사용. 스택/unique_ptr 생성 시 shared_from_this 미정의.)
// ------------------------------------------------------------

class ThreadPoolExecutor final : public IExecutor,
                                 public std::enable_shared_from_this<ThreadPoolExecutor>
{
public:
    struct CreateOptions
    {
        uint32_t WorkerCount = 0; // 0 → std::thread::hardware_concurrency()
        SlowJobOptions SlowJob{};
        std::string DebugName;
    };

    explicit ThreadPoolExecutor(CreateOptions options)
        : options_(std::move(options))
    {
        auto count = options_.WorkerCount ? options_.WorkerCount : std::thread::hardware_concurrency();

        workers_.reserve(count);
        for (auto i = 0; i < count; ++i)
        {
            workers_.emplace_back([this, i]
                                  {
                                      WorkerLoop(i);
                                  });
        }
    }

    ~ThreadPoolExecutor() override
    {
        Stop(false, Duration::zero());
    }

    ThreadPoolExecutor(const ThreadPoolExecutor&) = delete;
    ThreadPoolExecutor& operator=(const ThreadPoolExecutor&) = delete;

public:
    void Post(Callback fn) override
    {
        if (!fn)
            return;

        {
            std::lock_guard lock(mtx_);
            if (state_ != EnumJobQueueState::Running)
                return;
            ready_.push_back(std::move(fn));
            submittedCount_.fetch_add(1, std::memory_order_relaxed);
        }
        cv_.notify_one();
    }

    EnumJobQueueState GetState() const noexcept override
    {
        std::lock_guard lock(mtx_);
        return state_;
    }

    void Stop(bool drain, Duration drainTimeout) override
    {
        {
            std::lock_guard lock(mtx_);
            if (state_ == EnumJobQueueState::Stopped)
                return;
            state_ = EnumJobQueueState::Draining;
            if (!drain)
            {
                droppedOnStopCount_.fetch_add(ready_.size(), std::memory_order_relaxed);
                ready_.clear();
            }
        }
        cv_.notify_all();

        if (drain)
        {
            std::unique_lock lock(mtx_);
            auto pred = [&]
            {
                return ready_.empty();
            };

            if (drainTimeout == Duration::zero())
            {
                cv_.wait(lock, pred);
            }
            else if (const auto deadline = std::chrono::steady_clock::now() + drainTimeout;
                     !cv_.wait_until(lock, deadline, pred))
            {
                droppedOnStopCount_.fetch_add(ready_.size(), std::memory_order_relaxed);
                ready_.clear();
            }
        }

        for (auto& t : workers_)
        {
            if (t.joinable())
                t.join();
        }
        workers_.clear();

        {
            std::lock_guard lock(mtx_);
            state_ = EnumJobQueueState::Stopped;
        }
    }

    WorkerPoolStatsSnapshot GetStatsSnapshot() const override
    {
        WorkerPoolStatsSnapshot snap;
        {
            std::lock_guard lock(mtx_);
            snap.State = state_;
            snap.WorkerCount = static_cast<uint32_t>(workers_.size());
            snap.PendingCount = ready_.size();
        }
        snap.SubmittedCount = submittedCount_.load(std::memory_order_relaxed);
        snap.ExecutedCount = executedCount_.load(std::memory_order_relaxed);
        snap.FailedCount = failedCount_.load(std::memory_order_relaxed);
        return snap;
    }

private:
    void WorkerLoop(uint32_t /*index*/)
    {
        while (true)
        {
            Callback fn;
            {
                std::unique_lock lock(mtx_);
                cv_.wait(lock, [&]
                         {
                             return !ready_.empty() || state_ != EnumJobQueueState::Running;
                         });
                if (ready_.empty())
                {
                    if (state_ != EnumJobQueueState::Running)
                        return;
                    continue;
                }
                fn = std::move(ready_.front());
                ready_.pop_front();
            }

            const auto start = std::chrono::steady_clock::now();
            try
            {
                fn();
            }
            catch (...)
            {
                failedCount_.fetch_add(1, std::memory_order_relaxed);
                // swallow + log 위치. 로거 연결 전이므로 우선 카운터만.
            }
            executedCount_.fetch_add(1, std::memory_order_relaxed);

            if (options_.SlowJob.EnableWarningLog && options_.SlowJob.WarningThreshold > Duration::zero())
            {
                const auto elapsed = std::chrono::steady_clock::now() - start;
                if (elapsed > options_.SlowJob.WarningThreshold)
                {
                    // TODO: logger 연결 시 slow job warning.
                }
            }
        }
    }

private:
    CreateOptions options_;

    // 큐 영역
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::deque<Callback> ready_; // 워커가 꺼내 실행할 ready job 큐.

    // 상태 영역
    EnumJobQueueState state_ = EnumJobQueueState::Running;

    // 워커 영역
    std::vector<std::thread> workers_;

    // 통계 영역
    std::atomic<uint64_t> submittedCount_{};
    std::atomic<uint64_t> executedCount_{};
    std::atomic<uint64_t> failedCount_{};
    std::atomic<uint64_t> droppedOnStopCount_{}; // Stop(false) 또는 timeout 폐기 누계.
};

} // namespace psh::lib::job
