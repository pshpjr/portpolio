#pragma once

#include "clock/clock.h"

namespace psh::lib
{

class ManualClock final : public IClock
{
  public:
    double GetCurrentTime() const override
    {
        return currentTime_;
    }
    double GetDeltaTime() const override
    {
        return deltaTime_;
    }
    double GetIdleTime() const override
    {
        return idleTime_;
    }

    void SetCurrentTime(double time)
    {
        deltaTime_ = time - currentTime_;
        currentTime_ = time;
    }

    void Advance(double seconds)
    {
        currentTime_ += seconds;
        deltaTime_ = seconds;
    }

    void SetIdleTime(double idle)
    {
        idleTime_ = idle;
    }

  private:
    double currentTime_ = 0.0;
    double deltaTime_ = 0.0;
    double idleTime_ = 0.0;
};

} // namespace psh::lib
