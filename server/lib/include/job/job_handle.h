#pragma once

#include "entry.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace psh::lib::job
{

class JobHandle
{
public:
    JobHandle() = default;

    JobHandle(std::weak_ptr<Entry> entry, std::string_view debugLabel)
        : ref_(std::move(entry)),
          debugLabel_(debugLabel)
    {
    }

    [[nodiscard]] bool Expired() const noexcept
    {
        return ref_.expired();
    }

    [[nodiscard]] bool IsValid() const noexcept
    {
        return !Expired();
    }

    EnumCancelResult Cancel()
    {
        auto entry = ref_.lock();
        if (!entry)
            return EnumCancelResult::Expired;

        switch (entry->state_.load(std::memory_order_acquire))
        {
        case EnumJobState::Executing:
            return EnumCancelResult::AlreadyExecuting;

        case EnumJobState::Done:
        case EnumJobState::Failed:
            return EnumCancelResult::AlreadyFinished;

        case EnumJobState::Canceled:
            return EnumCancelResult::AlreadyCanceled;

        default:
            break;
        }

        if (entry->cancelRequested_.exchange(true, std::memory_order_acq_rel))
            return EnumCancelResult::AlreadyCanceled;

        return EnumCancelResult::Canceled;
    }

    [[nodiscard]] EnumJobState GetState() const
    {
        if (auto entry = ref_.lock())
            return entry->GetState();
        return EnumJobState::Expired;
    }

    [[nodiscard]] std::string_view GetDebugLabel() const noexcept
    {
        return debugLabel_;
    }

private:
    std::weak_ptr<Entry> ref_;
    std::string debugLabel_;
};

class RepeatHandle
{
public:
    RepeatHandle() = default;

    RepeatHandle(std::weak_ptr<Entry> entry,
                 Duration period,
                 EnumRepeatMode mode,
                 std::string_view debugLabel)
        : entry_(std::move(entry)),
          period_(period),
          mode_(mode),
          debugLabel_(debugLabel)
    {
    }

    [[nodiscard]] bool IsValid() const noexcept
    {
        return !entry_.expired();
    }

    [[nodiscard]] bool Expired() const noexcept
    {
        return entry_.expired();
    }

    EnumCancelResult Cancel()
    {
        auto entry = entry_.lock();
        if (!entry)
            return EnumCancelResult::Expired;

        if (entry->cancelRequested_.exchange(true, std::memory_order_acq_rel))
            return EnumCancelResult::AlreadyCanceled;

        return EnumCancelResult::Canceled;
    }

    bool Pause()
    {
        if (auto entry = entry_.lock())
        {
            if (entry->cancelRequested_.load(std::memory_order_acquire))
                return false;
            entry->paused_.store(true, std::memory_order_release);
            return true;
        }
        return false;
    }

    bool Resume()
    {
        if (auto entry = entry_.lock())
        {
            if (entry->cancelRequested_.load(std::memory_order_acquire))
                return false;
            entry->paused_.store(false, std::memory_order_release);
            return true;
        }
        return false;
    }

    bool ChangePeriod(Duration newPeriod)
    {
        if (auto entry = entry_.lock())
        {
            if (entry->cancelRequested_.load(std::memory_order_acquire))
                return false;
            entry->periodNs_.store(newPeriod.count(), std::memory_order_release);
            period_ = newPeriod;
            return true;
        }
        return false;
    }

    bool ChangeMode(EnumRepeatMode newMode)
    {
        if (auto entry = entry_.lock())
        {
            if (entry->cancelRequested_.load(std::memory_order_acquire))
                return false;
            entry->mode_.store(newMode, std::memory_order_release);
            mode_ = newMode;
            return true;
        }
        return false;
    }

    [[nodiscard]] Duration GetPeriod() const noexcept
    {
        return period_;
    }

    [[nodiscard]] EnumRepeatMode GetMode() const noexcept
    {
        return mode_;
    }

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
