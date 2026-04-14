#pragma once

#include "clock/clock.h"

#include <chrono>
#include <cstdint>

namespace psh::lib
{

class SteadyTickClock final : public IClock
{
public:
    SteadyTickClock() : start_(std::chrono::steady_clock::now()), lastTick_(start_) {}

    uint64_t GetCurrentTime() const override { return currentTime_; }
    uint64_t GetDeltaTime() const override   { return deltaTime_; }
    uint64_t GetIdleTime() const override    { return 0; }

    void Tick() {
        auto now     = std::chrono::steady_clock::now();
        auto elapsed = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count());
        auto delta   = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTick_).count());

        currentTime_ = elapsed;
        deltaTime_ = delta;
        lastTick_ = now;
    }

private:
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point lastTick_;
    uint64_t currentTime_ = 0;
    uint64_t deltaTime_   = 0;
};

} // namespace psh::lib
