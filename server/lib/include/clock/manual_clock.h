#pragma once

#include "clock/clock.h"

namespace psh::lib
{

class ManualClock final : public IClock
{
  public:
    uint64_t GetCurrentTime() const override
    {
        return currentTime_;
    }
    uint64_t GetDeltaTime() const override
    {
        return deltaTime_;
    }
    uint64_t GetIdleTime() const override
    {
        return idleTime_;
    }

    void SetCurrentTime(uint64_t time)
    {
        deltaTime_ = time - currentTime_;
        currentTime_ = time;
    }

    void Advance(uint64_t seconds)
    {
        currentTime_ += seconds;
        deltaTime_ = seconds;
    }

    void SetIdleTime(uint64_t idle)
    {
        idleTime_ = idle;
    }

  private:
    uint64_t currentTime_ = 0.0;
    uint64_t deltaTime_ = 0.0;
    uint64_t idleTime_ = 0.0;
};

} // namespace psh::lib
