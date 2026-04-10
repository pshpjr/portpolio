#include "clock/manual_clock.h"
#include "clock/steady_clock.h"
#include "clock/steady_tick_clock.h"

#include <gtest/gtest.h>
#include <thread>

namespace psh::lib {

// --- ManualClock ---

TEST(ManualClock, InitialValuesAreZero) {
    ManualClock clock;
    EXPECT_DOUBLE_EQ(clock.GetCurrentTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.GetDeltaTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.GetIdleTime(), 0.0);
}

TEST(ManualClock, Advance) {
    ManualClock clock;
    clock.Advance(1.5);

    EXPECT_DOUBLE_EQ(clock.GetCurrentTime(), 1.5);
    EXPECT_DOUBLE_EQ(clock.GetDeltaTime(), 1.5);

    clock.Advance(0.5);
    EXPECT_DOUBLE_EQ(clock.GetCurrentTime(), 2.0);
    EXPECT_DOUBLE_EQ(clock.GetDeltaTime(), 0.5);
}

TEST(ManualClock, SetCurrentTime) {
    ManualClock clock;
    clock.SetCurrentTime(10.0);

    EXPECT_DOUBLE_EQ(clock.GetCurrentTime(), 10.0);
    EXPECT_DOUBLE_EQ(clock.GetDeltaTime(), 10.0);

    clock.SetCurrentTime(12.0);
    EXPECT_DOUBLE_EQ(clock.GetCurrentTime(), 12.0);
    EXPECT_DOUBLE_EQ(clock.GetDeltaTime(), 2.0);
}

TEST(ManualClock, SetIdleTime) {
    ManualClock clock;
    clock.SetIdleTime(0.3);
    EXPECT_DOUBLE_EQ(clock.GetIdleTime(), 0.3);
}

// --- SteadyTickClock ---

TEST(SteadyTickClock, InitialValuesAreZero) {
    SteadyTickClock clock;
    EXPECT_DOUBLE_EQ(clock.GetCurrentTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.GetDeltaTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.GetIdleTime(), 0.0);
}

TEST(SteadyTickClock, TickAdvancesTime) {
    SteadyTickClock clock;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    clock.Tick();

    EXPECT_GT(clock.GetCurrentTime(), 0.0);
    EXPECT_GT(clock.GetDeltaTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.GetIdleTime(), 0.0);
}

TEST(SteadyTickClock, DeltaReflectsTickInterval) {
    SteadyTickClock clock;
    clock.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    clock.Tick();

    EXPECT_GE(clock.GetDeltaTime(), 0.015);
}

// --- SteadyClock ---

TEST(SteadyClock, StartsAndStops) {
    SteadyClock clock(1.0);
    clock.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    clock.Stop();

    EXPECT_GT(clock.GetCurrentTime(), 0.0);
    EXPECT_GT(clock.GetDeltaTime(), 0.0);
}

TEST(SteadyClock, DoubleStartIsNoop) {
    SteadyClock clock(1.0);
    clock.Start();
    clock.Start();  // 두 번째 호출은 무시
    clock.Stop();
}

TEST(SteadyClock, PolymorphicAccess) {
    ManualClock manual;
    SteadyTickClock tick;
    SteadyClock steady(1.0);

    IClock* clocks[] = {&manual, &tick, &steady};

    for (auto* c : clocks) {
        EXPECT_GE(c->GetCurrentTime(), 0.0);
        EXPECT_GE(c->GetDeltaTime(), 0.0);
        EXPECT_GE(c->GetIdleTime(), 0.0);
    }
}

}  // namespace psh::lib
