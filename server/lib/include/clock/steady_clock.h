#pragma once

#include "clock/clock.h"

#include <atomic>
#include <chrono>
#include <thread>

namespace psh::lib
{

class SteadyClock final : public IClock
{
public:
    explicit SteadyClock(std::chrono::nanoseconds tick_interval = std::chrono::milliseconds(1))
        : tick_interval_(tick_interval), start_(std::chrono::steady_clock::now()) {}

    ~SteadyClock() override
    {
        Stop();
    }

    SteadyClock(const SteadyClock&) = delete;
    SteadyClock& operator=(const SteadyClock&) = delete;
    SteadyClock(SteadyClock&&) = delete;
    SteadyClock& operator=(SteadyClock&&) = delete;

    uint64_t GetCurrentTime() const override
    {
        return current_time_.load(std::memory_order_acquire);
    }

    uint64_t GetDeltaTime() const override
    {
        return delta_time_.load(std::memory_order_acquire);
    }

    uint64_t GetIdleTime() const override
    {
        return idle_time_.load(std::memory_order_acquire);
    }

    void Start()
    {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true))
        {
            return;
        }
        thread_ = std::thread(
            [this]
            {
                Run();
            });
    }

    void Stop()
    {
        running_.store(false, std::memory_order_release);
        if (thread_.joinable())
        {
            thread_.join();
        }
    }

private:
    void Run()
    {
        auto last_tick = start_;
        const uint64_t idle_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(tick_interval_).count());

        while (running_.load(std::memory_order_acquire))
        {
            std::this_thread::sleep_for(tick_interval_);

            auto now     = std::chrono::steady_clock::now();
            auto elapsed = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count());
            auto delta   = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(now - last_tick).count());

            current_time_.store(elapsed, std::memory_order_release);
            delta_time_.store(delta, std::memory_order_release);
            idle_time_.store(idle_ns, std::memory_order_release);

            last_tick = now;
        }
    }

    std::chrono::nanoseconds tick_interval_;
    std::chrono::steady_clock::time_point start_;

    std::atomic<uint64_t> current_time_{};
    std::atomic<uint64_t> delta_time_{};
    std::atomic<uint64_t> idle_time_{};
    std::atomic<bool>   running_{false};
    std::thread         thread_;
};

}  // namespace psh::lib
