#pragma once

#include "clock/clock.h"

#include <atomic>
#include <chrono>
#include <thread>

namespace psh::lib {

class SteadyClock final : public IClock {
public:
    explicit SteadyClock(double tick_interval_ms = 1.0)
        : tick_interval_(std::chrono::duration<double, std::milli>(tick_interval_ms)),
          start_(std::chrono::steady_clock::now()) {}

    ~SteadyClock() override { Stop(); }

    SteadyClock(const SteadyClock&)            = delete;
    SteadyClock& operator=(const SteadyClock&) = delete;
    SteadyClock(SteadyClock&&)                 = delete;
    SteadyClock& operator=(SteadyClock&&)      = delete;

    double GetCurrentTime() const override {
        return current_time_.load(std::memory_order_acquire);
    }

    double GetDeltaTime() const override {
        return delta_time_.load(std::memory_order_acquire);
    }

    double GetIdleTime() const override {
        return idle_time_.load(std::memory_order_acquire);
    }

    void Start() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) {
            return;
        }
        thread_ = std::thread([this] { Run(); });
    }

    void Stop() {
        running_.store(false, std::memory_order_release);
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    void Run() {
        auto last_tick = start_;

        while (running_.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(tick_interval_);

            auto now     = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(now - start_).count();
            auto delta   = std::chrono::duration<double>(now - last_tick).count();
            auto idle    = std::chrono::duration<double>(tick_interval_).count();

            current_time_.store(elapsed, std::memory_order_release);
            delta_time_.store(delta, std::memory_order_release);
            idle_time_.store(idle, std::memory_order_release);

            last_tick = now;
        }
    }

    std::chrono::duration<double, std::milli> tick_interval_;
    std::chrono::steady_clock::time_point start_;

    std::atomic<double> current_time_{0.0};
    std::atomic<double> delta_time_{0.0};
    std::atomic<double> idle_time_{0.0};
    std::atomic<bool>   running_{false};
    std::thread         thread_;
};

}  // namespace psh::lib
