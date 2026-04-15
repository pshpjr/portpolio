#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>

namespace psh::lib::telemetry
{
// ------------------------------------------------------------
// 단순 카운터 기반 RateMeter. 도메인 무관 (job/network/io 어디든).
//
// PerSecond 는 직전 Snapshot() 이후의 평균. Snapshot 호출 빈도가 일정할수록
// 안정적이다 (예: 1초 주기 폴러). 첫 Snapshot 은 RateMeter 생성 시점부터 측정.
//
// Total 은 단조 증가 누적 카운트.
//
// hot path 가 아주 무겁다면 EWMA / sliding-window 로 교체 가능. 인터페이스는
// 그대로 유지된다.
// ------------------------------------------------------------

struct RateSnapshot
{
    double PerSecond = 0.0;
    uint64_t Total = 0;
};

class RateMeter
{
public:
    RateMeter() noexcept
        : lastSampleTime_(std::chrono::steady_clock::now())
    {
    }

    RateMeter(const RateMeter&) = delete;
    RateMeter& operator=(const RateMeter&) = delete;

    void Add(uint64_t delta = 1) noexcept
    {
        total_.fetch_add(delta, std::memory_order_relaxed);
    }

    [[nodiscard]] RateSnapshot Snapshot() const
    {
        using namespace std::chrono;

        RateSnapshot snap;
        snap.Total = total_.load(std::memory_order_relaxed);

        const auto now = steady_clock::now();

        std::lock_guard lock(sampleMtx_);

        if (auto elapsedNs = duration_cast<nanoseconds>(now - lastSampleTime_).count();
            elapsedNs > 0)
        {
            snap.PerSecond = static_cast<double>(snap.Total - lastSampledTotal_) * 1e9 / elapsedNs;
        }
        lastSampledTotal_ = snap.Total;
        lastSampleTime_ = now;
        return snap;
    }

private:
    std::atomic<uint64_t> total_{};

    mutable std::mutex sampleMtx_;
    mutable uint64_t lastSampledTotal_ = 0;
    mutable std::chrono::steady_clock::time_point lastSampleTime_;
};

} // namespace psh::lib::telemetry