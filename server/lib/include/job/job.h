#pragma once

#include "job_entry.h"
#include "timer_entry.h"

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

namespace psh::lib::job
{
// ============================================================
// JobHandle
//   - JobQueue::Post 또는 Timer::ScheduleAt/After 로 반환된다.
//   - weak_ptr<JobEntry | TimerEntry> 로 참조. 실패 시 invalid(monostate).
//   - Cancel / TryGetState 는 Entry 의 atomic 필드를 직접 조작/관측.
// ============================================================

class JobHandle
{
  public:
    JobHandle() = default;

    JobHandle(std::weak_ptr<JobEntry> entry, std::string_view debugLabel)
        : ref_(std::move(entry)), debugLabel_(debugLabel)
    {
    }

    JobHandle(std::weak_ptr<TimerEntry> entry, std::string_view debugLabel)
        : ref_(std::move(entry)), debugLabel_(debugLabel)
    {
    }

    [[nodiscard]] bool IsValid() const noexcept
    {
        return std::visit(
            [](const auto& r) -> bool {
                using T = std::decay_t<decltype(r)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                    return false;
                else
                    return !r.expired();
            },
            ref_);
    }

    [[nodiscard]] bool Expired() const noexcept
    {
        return std::visit(
            [](const auto& r) -> bool {
                using T = std::decay_t<decltype(r)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                    return false;
                else
                    return r.expired();
            },
            ref_);
    }

    EnumCancelResult Cancel()
    {
        return std::visit(
            [](auto& r) -> EnumCancelResult {
                using T = std::decay_t<decltype(r)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    return EnumCancelResult::Expired;
                }
                else
                {
                    auto entry = r.lock();
                    return CancelEntry(entry);
                }
            },
            ref_);
    }

    [[nodiscard]] EnumJobState TryGetState() const
    {
        return std::visit(
            [](const auto& r) -> EnumJobState {
                using T = std::decay_t<decltype(r)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                    return EnumJobState::Done;
                else
                {
                    auto entry = r.lock();
                    if (!entry)
                        return EnumJobState::Done;
                    return entry->State.load(std::memory_order_acquire);
                }
            },
            ref_);
    }

    [[nodiscard]] std::string_view GetDebugLabel() const noexcept
    {
        return debugLabel_;
    }

  private:
    template <class EntryPtr>
    static EnumCancelResult CancelEntry(const EntryPtr& entry)
    {
        if (!entry)
            return EnumCancelResult::Expired;

        const bool wasCanceled =
            entry->Canceled.exchange(true, std::memory_order_acq_rel);
        const auto state = entry->State.load(std::memory_order_acquire);

        if (wasCanceled)
        {
            if (state == EnumJobState::Done || state == EnumJobState::Failed)
                return EnumCancelResult::AlreadyFinished;
            return EnumCancelResult::AlreadyCanceled;
        }

        if (state == EnumJobState::Executing)
            return EnumCancelResult::AlreadyExecuting;
        if (state == EnumJobState::Done || state == EnumJobState::Failed)
            return EnumCancelResult::AlreadyFinished;
        return EnumCancelResult::Canceled;
    }

  private:
    std::variant<std::monostate,
                 std::weak_ptr<JobEntry>,
                 std::weak_ptr<TimerEntry>> ref_;
    std::string debugLabel_;
};

// ============================================================
// RepeatHandle
//   - Timer::ScheduleRepeat 에서 반환.
//   - period / mode / debugLabel 은 핸들 로컬 캐시. Change* 성공 시 로컬 갱신.
//   - Pause/Resume 은 Entry 의 Paused atomic 조작. 다음 회차부터 반영.
//     (Paused 인 채로 만료된 회차는 dispatch 를 skip 하고 재등록만 한다.)
// ============================================================

class RepeatHandle
{
  public:
    RepeatHandle() = default;

    RepeatHandle(std::weak_ptr<TimerEntry> entry,
                 Duration period,
                 EnumRepeatMode mode,
                 std::string_view debugLabel)
        : entry_(std::move(entry))
        , period_(period)
        , mode_(mode)
        , debugLabel_(debugLabel)
    {
    }

    [[nodiscard]] bool IsValid() const noexcept { return !entry_.expired(); }
    [[nodiscard]] bool Expired() const noexcept { return entry_.expired(); }

    bool Cancel()
    {
        if (auto e = entry_.lock())
        {
            e->Canceled.store(true, std::memory_order_release);
            return true;
        }
        return false;
    }

    bool Pause()
    {
        if (auto e = entry_.lock())
        {
            if (e->Canceled.load(std::memory_order_acquire))
                return false;
            e->Paused.store(true, std::memory_order_release);
            return true;
        }
        return false;
    }

    bool Resume()
    {
        if (auto e = entry_.lock())
        {
            if (e->Canceled.load(std::memory_order_acquire))
                return false;
            e->Paused.store(false, std::memory_order_release);
            return true;
        }
        return false;
    }

    bool ChangePeriod(Duration newPeriod)
    {
        if (auto e = entry_.lock())
        {
            if (e->Canceled.load(std::memory_order_acquire))
                return false;
            e->PeriodNs.store(newPeriod.count(), std::memory_order_release);
            period_ = newPeriod;
            return true;
        }
        return false;
    }

    bool ChangeMode(EnumRepeatMode newMode)
    {
        if (auto e = entry_.lock())
        {
            if (e->Canceled.load(std::memory_order_acquire))
                return false;
            e->Mode.store(newMode, std::memory_order_release);
            mode_ = newMode;
            return true;
        }
        return false;
    }

    [[nodiscard]] Duration GetPeriod() const noexcept { return period_; }
    [[nodiscard]] EnumRepeatMode GetMode() const noexcept { return mode_; }
    [[nodiscard]] std::string_view GetDebugLabel() const noexcept
    {
        return debugLabel_;
    }

  private:
    std::weak_ptr<TimerEntry> entry_;
    Duration period_{};
    EnumRepeatMode mode_{EnumRepeatMode::FixedDelay};
    std::string debugLabel_;
};

} // namespace psh::lib::job
