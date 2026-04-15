#pragma once

#include "entry.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace psh::lib::job
{
// ============================================================
// JobHandle
//   - JobQueue::Post / Timer::ScheduleAt / ScheduleAfter 에서 반환.
//   - weak_ptr<Entry> 로만 참조. 기본 생성은 invalid.
//   - Cancel / GetState 는 Entry 의 atomic 필드를 직접 조작/관측.
//   - Entry.Kind 로 Timer/Plain 구분이 필요하면 참조 가능.
// ============================================================

class JobHandle
{
public:
    JobHandle() = default;

    JobHandle(std::weak_ptr<Entry> entry, std::string_view debugLabel)
        : ref_(std::move(entry)), debugLabel_(debugLabel)
    {
    }

    [[nodiscard]] bool Expired() const noexcept { return ref_.expired(); }
    [[nodiscard]] bool IsValid() const noexcept { return !Expired(); }

    EnumCancelResult Cancel()
    {
        auto entry = ref_.lock();
        if (!entry)
            return EnumCancelResult::Expired;

        if (entry->Canceled.exchange(true, std::memory_order_acq_rel))
            return EnumCancelResult::AlreadyCanceled;

        switch (entry->State.load(std::memory_order_acquire))
        {
        case EnumJobState::Executing:
            return EnumCancelResult::AlreadyExecuting;

        case EnumJobState::Done: [[fallthrough]];
        case EnumJobState::Failed:
            return EnumCancelResult::AlreadyFinished;

        case EnumJobState::Canceled:
            //  불가능한 상황
            break;
        //  이 외의 경우에는 취소가 가능하다
        default:
            break;
        }

        return EnumCancelResult::Canceled;
    }

    [[nodiscard]] EnumJobState GetState() const
    {
        if (auto entry = ref_.lock())
            return entry->State.load(std::memory_order_acquire);

        return EnumJobState::Done;
    }

    [[nodiscard]] std::string_view GetDebugLabel() const noexcept
    {
        return debugLabel_;
    }

  private:
    std::weak_ptr<Entry> ref_;
    std::string debugLabel_;
};

// ============================================================
// RepeatHandle
//   - Timer::ScheduleRepeat 전용. 대상 Entry 는 Kind == Timer 이고 Period 를 가진다.
//   - period / mode / debugLabel 은 핸들 로컬 캐시. Change* 성공 시 로컬 갱신.
//   - Pause/Resume 은 Entry.Paused atomic. 다음 회차부터 반영.
//     (Paused 인 채로 만료된 회차는 dispatch 를 skip 하고 재등록만 수행)
// ============================================================

class RepeatHandle
{
  public:
    RepeatHandle() = default;

    RepeatHandle(std::weak_ptr<Entry> entry,
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
    std::weak_ptr<Entry> entry_;
    Duration period_{};
    EnumRepeatMode mode_{EnumRepeatMode::FixedDelay};
    std::string debugLabel_;
};

} // namespace psh::lib::job
