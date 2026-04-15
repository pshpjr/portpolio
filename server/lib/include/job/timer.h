#pragma once

#include "entry.h"
#include "job_handle.h"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>

namespace psh::lib::job
{

/*
 * Timer — 시각 기반 콜백 스케줄러.
 *
 * 특정 시간에 특정 함수를 호출한다.
 * 해상도(TickResolution)를 기반으로 버킷 단위로 관리하며,
 * 해상도 이하의 정확도는 보장하지 않는다.
 *
 * Executor 를 모른다. 내부 watcher 스레드가 콜백을 직접 실행한다.
 *
 * 인터페이스:
 *   PostAt(time, fn)  — 지정 시각에 콜백 예약.
 *   Start() / Stop()  — 수명 관리.
 *   GetPendingCount() / GetTickResolution() — 관측.
 *
 * 시간 소스:
 *   기본은 steady_clock::now(). CreateOptions::NowProvider 로 주입 가능.
 *   테스트 시 FakeClock 람다를 넣으면 외부에서 시간을 제어할 수 있다.
 *
 * 해상도: 기본 틱 10ms.
 */

struct TimerCreateOptions
{
    Duration TickResolution = std::chrono::milliseconds(10);
    // NowProvider 가 비어있으면 std::chrono::steady_clock::now() 사용.
    // 테스트는 FakeClock 람다를 주입.
    std::function<TimePoint()> NowProvider;
    std::string DebugName;
};

class Timer
{
  public:
    using Callback = std::function<void()>;
    using CreateOptions = TimerCreateOptions;

    explicit Timer(CreateOptions options = {})
        : options_(std::move(options)), tickResolution_(options_.TickResolution)
    {
        if (tickResolution_.count() <= 0)
            tickResolution_ = std::chrono::milliseconds(10);

        if (!options_.NowProvider)
            nowProvider_ = []
            {
                return std::chrono::steady_clock::now();
            };
        else
            nowProvider_ = options_.NowProvider;
    }

    ~Timer()
    {
        Stop();
    }

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

  public:
    void Start()
    {
        if (started_.exchange(true))
            return;
        stopping_.store(false, std::memory_order_release);
        watcher_ = std::thread(
            [this]
            {
                WatcherLoop();
            });
    }

    void Stop()
    {
        if (stopping_.load(std::memory_order_acquire))
            return;
        stopping_.store(true, std::memory_order_release);

        if (watcher_.joinable())
            watcher_.join();

        std::lock_guard lock(mtx_);
        for (auto& list : buckets_ | std::views::values)
        {
            for (auto& entry : list)
            {
                entry->cancelRequested_.store(true, std::memory_order_release);
                entry->state_.store(EnumJobState::Canceled, std::memory_order_release);
            }
        }
        buckets_.clear();
        occupied_.clear();
    }

  public:
    JobHandle PostAt(TimePoint at, Callback fn, std::string_view label = {})
    {
        if (!fn)
            return JobHandle{};

        if (stopping_.load(std::memory_order_acquire))
            return JobHandle{};

        auto entry = std::make_shared<Entry>(NextId(), label, std::move(fn), at);
        entry->bucket_ = BucketOf(at);
        entry->state_.store(EnumJobState::WaitingTimer, std::memory_order_relaxed);

        {
            std::lock_guard lock(mtx_);
            Insert(entry);
        }

        return JobHandle{entry, label};
    }

  public:
    [[nodiscard]] size_t GetPendingCount() const noexcept
    {
        std::lock_guard lock(mtx_);
        size_t total = 0;
        for (const auto& list : buckets_ | std::views::values)
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
        const auto ns = std::chrono::duration_cast<Duration>(tp.time_since_epoch()).count();
        return static_cast<BucketKey>(ns / tickResolution_.count());
    }

    [[nodiscard]] TimePoint BucketTime(BucketKey bucket) const
    {
        return TimePoint(Duration(static_cast<Duration::rep>(bucket) * tickResolution_.count()));
    }

    [[nodiscard]] TimePoint Now() const
    {
        return nowProvider_();
    }

    EntryId NextId() noexcept
    {
        return nextId_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    void Insert(const std::shared_ptr<Entry>& entry)
    {
        const BucketKey bucket = entry->bucket_;
        buckets_[bucket].push_back(entry);
        occupied_.insert(bucket);
    }

    void WatcherLoop()
    {
        while (!stopping_.load(std::memory_order_acquire))
        {
            std::this_thread::sleep_for(tickResolution_);

            const BucketKey currentBucket = BucketOf(Now());

            std::list<std::shared_ptr<Entry>> fired;
            {
                std::lock_guard lock(mtx_);
                while (!occupied_.empty() && *occupied_.begin() <= currentBucket)
                {
                    const BucketKey bucket = *occupied_.begin();
                    if (auto it = buckets_.find(bucket); it != buckets_.end())
                    {
                        fired.splice(fired.end(), it->second);
                        buckets_.erase(it);
                    }
                    occupied_.erase(occupied_.begin());
                }
            }

            for (auto& entry : fired)
            {
                if (entry->cancelRequested_.load(std::memory_order_acquire))
                {
                    entry->state_.store(EnumJobState::Canceled, std::memory_order_release);
                    continue;
                }

                entry->state_.store(EnumJobState::Executing, std::memory_order_release);
                try
                {
                    entry->fn_();
                    entry->fn_ = nullptr;
                    entry->state_.store(EnumJobState::Done, std::memory_order_release);
                }
                catch (...)
                {
                    entry->fn_ = nullptr;
                    entry->state_.store(EnumJobState::Failed, std::memory_order_release);
                }
            }
        }
    }

  private:
    // 설정 영역
    CreateOptions options_;
    Duration tickResolution_;
    std::function<TimePoint()> nowProvider_;

    // 버킷 영역
    mutable std::mutex mtx_;
    std::unordered_map<BucketKey, std::list<std::shared_ptr<Entry>>> buckets_;
    std::set<BucketKey> occupied_;

    // 상태 영역
    std::atomic<EntryId> nextId_{};
    std::atomic<bool> stopping_{true};
    std::atomic<bool> started_{false};
    std::thread watcher_;
};

} // namespace psh::lib::job