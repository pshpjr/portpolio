#pragma once

#include "executor.h"
#include "job.h"
#include "job_queue.h"
#include "entry.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>

namespace psh::lib::job
{
// ------------------------------------------------------------
// Timer — 시각 기반 Job 스케줄러.
//
// 설계 원칙:
//   - 생성 시 shared_ptr<IExecutor> 를 바인딩. 바인딩된 executor 가 기본 대상.
//   - JobQueue 대상 스케줄은 별도 overload. 내부에 weak_ptr<JobQueue> 를 캡처한 DispatchFn 으로 보관.
//   - Watcher 스레드는 사용자 콜백을 직접 실행하지 않고 Executor / JobQueue 로 위임.
//   - Paused 인 repeat 엔트리는 dispatch 만 skip 하고 재등록은 수행 → Resume 즉시 다음 회차부터 정상화.
//
// 생성 규약:
//   std::make_shared<Timer>(executor, options)
//   enable_shared_from_this 사용.
// ------------------------------------------------------------

class Timer : public std::enable_shared_from_this<Timer>
{
  public:
    using Callback = IExecutor::Callback;

    struct CreateOptions
    {
        Duration TickResolution = std::chrono::milliseconds(10);
        // NowProvider 가 비어있으면 std::chrono::steady_clock::now() 사용.
        // 테스트는 FakeClock 람다를 주입.
        std::function<TimePoint()> NowProvider;
        std::string DebugName;
    };

    Timer(std::shared_ptr<IExecutor> executor, CreateOptions options = {})
        : executor_(std::move(executor))
        , options_(std::move(options))
        , tickResolution_(options_.TickResolution)
    {
        if (!options_.NowProvider)
            nowProvider_ = [] { return std::chrono::steady_clock::now(); };
        else
            nowProvider_ = options_.NowProvider;
    }

    ~Timer() { Stop(); }

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

  public:
    void Start()
    {
        bool expected = false;
        if (!started_.compare_exchange_strong(expected, true))
            return;
        stopping_.store(false, std::memory_order_release);
        watcher_ = std::thread([this] { WatcherLoop(); });
    }

    void Stop()
    {
        {
            std::lock_guard lock(mtx_);
            if (stopping_.load(std::memory_order_acquire))
                return;
            stopping_.store(true, std::memory_order_release);
        }
        cv_.notify_all();
        if (watcher_.joinable())
            watcher_.join();

        std::lock_guard lock(mtx_);
        for (auto& [bucket, list] : buckets_)
        {
            for (auto& entry : list)
            {
                entry->Canceled.store(true, std::memory_order_release);
                entry->State.store(EnumJobState::Canceled,
                                   std::memory_order_release);
            }
        }
        buckets_.clear();
        occupied_.clear();
    }

  public:
    // === 바인딩된 Executor 대상 ===
    JobHandle ScheduleAt(TimePoint at, Callback fn, std::string_view label = {})
    {
        return ScheduleAtImpl(at, std::move(fn), label, MakeExecutorDispatch());
    }

    JobHandle ScheduleAfter(Duration delay,
                            Callback fn,
                            std::string_view label = {})
    {
        return ScheduleAt(Now() + delay, std::move(fn), label);
    }

    RepeatHandle ScheduleRepeat(Duration period,
                                Callback fn,
                                EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                                bool executeNow = false,
                                std::string_view label = {})
    {
        return ScheduleRepeatImpl(period, std::move(fn), mode, executeNow, label,
                                  MakeExecutorDispatch());
    }

    // === JobQueue 대상 ===
    JobHandle ScheduleAt(const std::shared_ptr<JobQueue>& target,
                         TimePoint at,
                         Callback fn,
                         std::string_view label = {})
    {
        return ScheduleAtImpl(at, std::move(fn), label,
                              MakeQueueDispatch(target));
    }

    JobHandle ScheduleAfter(const std::shared_ptr<JobQueue>& target,
                            Duration delay,
                            Callback fn,
                            std::string_view label = {})
    {
        return ScheduleAt(target, Now() + delay, std::move(fn), label);
    }

    RepeatHandle ScheduleRepeat(const std::shared_ptr<JobQueue>& target,
                                Duration period,
                                Callback fn,
                                EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                                bool executeNow = false,
                                std::string_view label = {})
    {
        return ScheduleRepeatImpl(period, std::move(fn), mode, executeNow, label,
                                  MakeQueueDispatch(target));
    }

  public:
    [[nodiscard]] size_t GetPendingCount() const noexcept
    {
        std::lock_guard lock(mtx_);
        size_t total = 0;
        for (const auto& [bucket, list] : buckets_)
            total += list.size();
        return total;
    }

    [[nodiscard]] Duration GetTickResolution() const noexcept
    {
        return tickResolution_;
    }

  private:
    using BucketKey = uint64_t;

    [[nodiscard]] BucketKey BucketOf(TimePoint tp) const
    {
        const auto nsSinceEpoch =
            std::chrono::duration_cast<Duration>(tp.time_since_epoch()).count();
        const auto tickNs = tickResolution_.count();
        if (tickNs <= 0)
            return static_cast<BucketKey>(nsSinceEpoch);
        // floor division
        auto q = nsSinceEpoch / tickNs;
        if (nsSinceEpoch % tickNs < 0)
            --q;
        return static_cast<BucketKey>(q);
    }

    [[nodiscard]] TimePoint BucketTime(BucketKey bucket) const
    {
        return TimePoint(Duration(static_cast<Duration::rep>(bucket)
                                  * tickResolution_.count()));
    }

    [[nodiscard]] TimePoint Now() const { return nowProvider_(); }

    Entry::DispatchFn MakeExecutorDispatch()
    {
        std::weak_ptr<IExecutor> weakExec = executor_;
        return [weakExec](IExecutor::Callback cb,
                          std::string_view /*label*/) -> bool {
            if (auto exec = weakExec.lock())
                return exec->Post(std::move(cb));
            return false;
        };
    }

    Entry::DispatchFn
    MakeQueueDispatch(const std::shared_ptr<JobQueue>& target)
    {
        std::weak_ptr<JobQueue> weakQ = target;
        return [weakQ](IExecutor::Callback cb,
                       std::string_view label) -> bool {
            if (auto q = weakQ.lock())
            {
                auto handle = q->Post(std::move(cb), label);
                return handle.IsValid();
            }
            return false;
        };
    }

    EntryId NextId() noexcept
    {
        return nextId_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    JobHandle ScheduleAtImpl(TimePoint at,
                             Callback fn,
                             std::string_view label,
                             Entry::DispatchFn dispatch)
    {
        if (!fn)
            return JobHandle{};

        auto entry = std::make_shared<Entry>(
            NextId(), label, std::move(fn), at, BucketOf(at),
            std::move(dispatch));

        bool wasNewMin = false;
        {
            std::lock_guard lock(mtx_);
            if (stopping_.load(std::memory_order_acquire))
                return JobHandle{};
            wasNewMin = Insert(entry);
        }
        if (wasNewMin)
            cv_.notify_one();

        return JobHandle{std::weak_ptr<Entry>(entry), label};
    }

    RepeatHandle ScheduleRepeatImpl(Duration period,
                                    Callback fn,
                                    EnumRepeatMode mode,
                                    bool executeNow,
                                    std::string_view label,
                                    Entry::DispatchFn dispatch)
    {
        if (!fn || period <= Duration::zero())
            return RepeatHandle{};

        const TimePoint at = executeNow ? Now() : Now() + period;
        auto entry = std::make_shared<Entry>(
            NextId(), label, std::move(fn), at, BucketOf(at),
            std::move(dispatch));

        entry->Period = period;
        entry->PeriodNs.store(period.count(), std::memory_order_release);
        entry->Mode.store(mode, std::memory_order_release);

        bool wasNewMin = false;
        {
            std::lock_guard lock(mtx_);
            if (stopping_.load(std::memory_order_acquire))
                return RepeatHandle{};
            wasNewMin = Insert(entry);
        }
        if (wasNewMin)
            cv_.notify_one();

        return RepeatHandle{std::weak_ptr<Entry>(entry), period, mode, label};
    }

    // mtx_ 보유 상태로 호출. 새 엔트리가 최소 bucket 을 앞당겼는지 반환.
    bool Insert(const std::shared_ptr<Entry>& entry)
    {
        const BucketKey bucket = entry->Bucket;
        buckets_[bucket].push_back(entry);
        const bool wasNewMin =
            occupied_.empty() || bucket < *occupied_.begin();
        occupied_.insert(bucket);
        return wasNewMin;
    }

    void WatcherLoop()
    {
        while (!stopping_.load(std::memory_order_acquire))
        {
            BucketKey nextBucket = 0;
            {
                std::unique_lock lock(mtx_);
                cv_.wait(lock, [this] {
                    return stopping_.load(std::memory_order_acquire)
                           || !occupied_.empty();
                });
                if (stopping_.load(std::memory_order_acquire))
                    return;
                nextBucket = *occupied_.begin();
            }

            const TimePoint fireAt = BucketTime(nextBucket);
            if (Now() < fireAt)
            {
                std::unique_lock lock(mtx_);
                cv_.wait_until(lock, fireAt, [this, nextBucket] {
                    return stopping_.load(std::memory_order_acquire)
                           || (!occupied_.empty()
                               && *occupied_.begin() < nextBucket);
                });
                continue;
            }

            std::list<std::shared_ptr<Entry>> fired;
            {
                std::lock_guard lock(mtx_);
                if (auto it = buckets_.find(nextBucket); it != buckets_.end())
                {
                    fired.splice(fired.end(), it->second);
                    buckets_.erase(it);
                }
                occupied_.erase(nextBucket);
            }

            for (auto& entry : fired)
            {
                if (entry->Canceled.load(std::memory_order_acquire))
                {
                    entry->State.store(EnumJobState::Canceled,
                                       std::memory_order_release);
                    continue;
                }

                const bool paused =
                    entry->Paused.load(std::memory_order_acquire);

                if (!paused)
                {
                    entry->State.store(EnumJobState::Queued,
                                       std::memory_order_release);
                    const bool ok = entry->Dispatch(entry->Fn, entry->DebugLabel);
                    if (!ok)
                    {
                        entry->Canceled.store(true, std::memory_order_release);
                        entry->State.store(EnumJobState::Canceled,
                                           std::memory_order_release);
                        continue;
                    }
                }

                if (entry->Period.has_value()
                    && !entry->Canceled.load(std::memory_order_acquire))
                {
                    const Duration period = Duration(
                        entry->PeriodNs.load(std::memory_order_acquire));
                    const EnumRepeatMode mode =
                        entry->Mode.load(std::memory_order_acquire);
                    const TimePoint nextAt =
                        ComputeNext(entry->ScheduledAt, mode, period, Now());
                    entry->ScheduledAt = nextAt;
                    entry->Bucket = BucketOf(nextAt);

                    bool wasNewMin = false;
                    {
                        std::lock_guard lock(mtx_);
                        wasNewMin = Insert(entry);
                    }
                    if (wasNewMin)
                        cv_.notify_one();
                }
            }
        }
    }

    static TimePoint ComputeNext(TimePoint scheduledAt,
                                 EnumRepeatMode mode,
                                 Duration period,
                                 TimePoint now)
    {
        if (mode == EnumRepeatMode::FixedDelay)
            return now + period;
        // FixedRate: catch-up 누적 없음.
        TimePoint t = scheduledAt + period;
        if (t <= now)
            t = now;
        return t;
    }

  private:
    std::shared_ptr<IExecutor> executor_;
    CreateOptions options_;
    Duration tickResolution_;
    std::function<TimePoint()> nowProvider_;

    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::unordered_map<BucketKey, std::list<std::shared_ptr<Entry>>> buckets_;
    std::set<BucketKey> occupied_;

    std::atomic<EntryId> nextId_{0};
    std::atomic<bool> stopping_{true};   // Start 호출 전에는 정지 상태
    std::atomic<bool> started_{false};
    std::thread watcher_;
};

} // namespace psh::lib::job
