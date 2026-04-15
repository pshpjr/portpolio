#pragma once

#include "executor.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace psh::lib::job
{
// ------------------------------------------------------------
// Entry — JobQueue / Timer 가 공유하는 단일 엔트리 타입.
//
// 두 경로에서 shared_ptr<Entry> 로 소유되고, Handle(JobHandle / RepeatHandle) 은
// weak_ptr<Entry> 로만 참조한다. 경로 구분은 내부 Kind 로만 하며 외부 노출은 없다.
//
//   A) JobQueue::Post        — 즉시 실행 대상.
//                              ScheduledAt = 제출 시각.
//                              Dispatch / Bucket / Period 는 사용하지 않는다.
//
//   B) Timer::Schedule*      — 시각 도달 후 Dispatch 로 대상(Executor / JobQueue)
//                              에 재위임. Period 가 있으면 Repeat.
//
// Cancel 은 cancelRequested_ atomic flip 만으로 처리(lazy cancel). 실행 직전에
// 관측하여 skip 한다. bucket/queue 에서의 erase 는 수행하지 않는다.
//
// 모든 필드는 private. 외부 관측은 Get*/Is* 게터로만. mutation 은 friend
// (JobQueue / Timer / JobHandle / RepeatHandle) 에서 직접 필드 접근한다.
// ------------------------------------------------------------

class Entry
{
private:
    // 엔트리 타입
    enum class EnumKind : uint8_t
    {
        Plain, // JobQueue::Post 로 생성. 즉시 실행.
        Timer  // Timer::Schedule* 로 생성. 시각 도달 후 Dispatch.
    };

public:
    using DispatchFn = std::function<void(IExecutor::Callback, std::string_view /*label*/)>;

    // JobQueue 경로용 생성자. 초기 상태 Queued.
    Entry(EntryId id,
          std::string_view debugLabel,
          IExecutor::Callback fn,
          TimePoint scheduledAt)
        : id_(id),
          fn_(std::move(fn)),
          scheduledAt_(scheduledAt),
          debugLabel_(debugLabel),
          kind_(EnumKind::Plain)
    {
    }

    // Timer 경로용 생성자. 초기 상태 WaitingTimer.
    Entry(EntryId id,
          std::string_view debugLabel,
          IExecutor::Callback fn,
          TimePoint scheduledAt,
          uint64_t bucket,
          DispatchFn dispatch)
        : id_(id),
          fn_(std::move(fn)),
          scheduledAt_(scheduledAt),
          debugLabel_(debugLabel),
          bucket_(bucket),
          dispatch_(std::move(dispatch)),
          kind_(EnumKind::Timer)
    {
        state_.store(EnumJobState::WaitingTimer, std::memory_order_relaxed);
    }

    // ------------------------------------------------------------
    // 외부 관측 API (모두 read-only)
    // ------------------------------------------------------------
    [[nodiscard]] EntryId GetId() const noexcept
    {
        return id_;
    }
    [[nodiscard]] std::string_view GetDebugLabel() const noexcept
    {
        return debugLabel_;
    }
    [[nodiscard]] TimePoint GetScheduledAt() const noexcept
    {
        return scheduledAt_;
    }
    [[nodiscard]] uint64_t GetBucket() const noexcept
    {
        return bucket_;
    }
    [[nodiscard]] std::optional<Duration> GetPeriod() const noexcept
    {
        return period_;
    }
    [[nodiscard]] EnumRepeatMode GetMode() const noexcept
    {
        return mode_.load(std::memory_order_acquire);
    }
    [[nodiscard]] bool IsPaused() const noexcept
    {
        return paused_.load(std::memory_order_acquire);
    }
    [[nodiscard]] bool IsCancelRequested() const noexcept
    {
        return cancelRequested_.load(std::memory_order_acquire);
    }

    // cancelRequested_ intent 와 state_ progress 를 합쳐 effective lifecycle 반환.
    //   - cancelRequested_ == true 이고 아직 실행 전(Queued/WaitingTimer) → Canceled.
    //   - 그 외엔 state_ 그대로 (Executing/Done/Failed/Canceled/Rejected).
    [[nodiscard]] EnumJobState GetState() const noexcept
    {
        const auto state = state_.load(std::memory_order_acquire);
        if (cancelRequested_.load(std::memory_order_acquire) &&
            (state == EnumJobState::Queued || state == EnumJobState::WaitingTimer))
        {
            return EnumJobState::Canceled;
        }
        return state;
    }

private:
    // ------------------------------------------------------------
    // 식별/콜백 (불변)
    // ------------------------------------------------------------
    const EntryId id_;
    IExecutor::Callback fn_;
    TimePoint scheduledAt_; // 제출 시각. Timer Repeat 재예약 시 갱신.
    const std::string debugLabel_;

    // ------------------------------------------------------------
    // Timer 영역 (Plain 에서는 미사용)
    // ------------------------------------------------------------
    uint64_t bucket_ = 0; // timer 의 버킷 키. Repeat 재예약 시 갱신.
    DispatchFn dispatch_;

    // ------------------------------------------------------------
    // Repeat 영역
    // ------------------------------------------------------------
    std::optional<Duration> period_; // nullopt → 1회성. Timer 에서만 세팅.
    std::atomic<Duration::rep> periodNs_{};
    std::atomic<EnumRepeatMode> mode_{EnumRepeatMode::FixedDelay};
    std::atomic<bool> paused_{false};

    // ------------------------------------------------------------
    // 라이프사이클
    //   state_           — drain/execute 단일 writer.
    //   cancelRequested_ — 외부 Cancel intent. GetState() 가 합쳐 보여준다.
    // ------------------------------------------------------------
    std::atomic<EnumJobState> state_{EnumJobState::Queued};
    std::atomic<bool> cancelRequested_{false};

    EnumKind kind_;

    friend class JobQueue;
    friend class Timer;
    friend class JobHandle;
    friend class RepeatHandle;
};

} // namespace psh::lib::job
