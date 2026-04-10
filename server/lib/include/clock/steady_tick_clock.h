#pragma once

#include "clock/clock.h"

#include <chrono>

namespace psh::lib {

class SteadyTickClock final : public IClock {
public:
    SteadyTickClock()
        : start_(std::chrono::steady_clock::now()),
          lastTick_(start_) {}

    double GetCurrentTime() const override { return currentTime_; }
    double GetDeltaTime() const override   { return deltaTime_; }
    double GetIdleTime() const override    { return 0.0; }

    void Tick() {
        auto now     = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - start_).count();
        auto delta   = std::chrono::duration<double>(now - lastTick_).count();

        currentTime_ = elapsed;
        deltaTime_   = delta;
        lastTick_    = now;
    }

private:
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point lastTick_;
    double currentTime_ = 0.0;
    double deltaTime_   = 0.0;
};

}  // namespace psh::lib
