#pragma once

#include "utils/atomic_util.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace psh::lib::telemetry
{
// ------------------------------------------------------------
//   각 버킷은 (LowerBound, UpperBound] 의 닫힌 상한 구간. 단, 첫 버킷의
//   LowerBound 는 0 이고 0 도 포함한다. 마지막 버킷은 UpperBound = UINT64_MAX
//   (자동 sentinel) 로 모든 잔여 값을 흡수한다.
//
// [구현 정책]
//   1차 구현: **버킷마다 std::atomic<uint64_t> 카운터**. 총합/최댓값/총횟수도
//   원자 변수로 별도 보관. 카운트는 lossless (IsApproximate=false). Snapshot 은
//   per-counter load 를 모아 만들기 때문에 버킷 간 시점이 살짝 어긋나는 일은
//   있을 수 있으나, 카운트 자체는 누락/중복 없이 정확하다.
//
//   향후 hot path 호출 빈도가 임계를 넘으면 다음 형태로 교체 가능. 인터페이스
//   (Record / Snapshot) 는 그대로 유지된다:
//     - per-thread shard + Snapshot 시 merge      (false sharing 회피)
//     - HDR-style log-linear 버킷                  (메모리 효율)
//   이 두 변형은 머지 시점 오차를 허용하므로 IsApproximate=true 를 반환하면 됨.
// ------------------------------------------------------------

struct HistogramBucket
{
    uint64_t LowerBound = 0; // inclusive
    uint64_t UpperBound = 0; // inclusive
    uint64_t Count = 0;
};

struct HistogramSnapshot
{
    std::vector<HistogramBucket> Buckets;
    uint64_t TotalCount = 0;
    uint64_t Sum = 0;
    uint64_t Maximum = 0;
    bool IsApproximate = false;
};

// 분포 요약
struct DistributionSummary
{
    uint64_t Count = 0;
    uint64_t Average = 0;
    uint64_t Maximum = 0;
    uint64_t P50 = 0;
    uint64_t P95 = 0;
    uint64_t P99 = 0;
};

class Histogram
{
public:
    // upperBounds: 오름차순. 중복 없음. 마지막이 UINT64_MAX 가 아니면 자동 추가.
    explicit Histogram(std::vector<uint64_t> upperBounds)
        : upperBounds_(std::move(upperBounds))
    {
        if (upperBounds_.empty() || upperBounds_.back() != std::numeric_limits<uint64_t>::max())
        {
            upperBounds_.push_back(std::numeric_limits<uint64_t>::max());
        }

        counts_ = std::vector<std::atomic<uint64_t>>(upperBounds_.size());
    }

    Histogram(const Histogram&) = delete;
    Histogram& operator=(const Histogram&) = delete;

    void Record(uint64_t value) noexcept
    {
        // 내가 속한 버킷 인덱스
        const size_t idx = std::ranges::lower_bound(upperBounds_, value) - upperBounds_.begin();

        counts_[idx].fetch_add(1, std::memory_order_relaxed);
        totalCount_.fetch_add(1, std::memory_order_relaxed);
        sum_.fetch_add(value, std::memory_order_relaxed);

        //  최대값 갱신
        utils::FetchMax(max_, value, std::memory_order_relaxed);
    }

    [[nodiscard]] DistributionSummary SummarizeAndReset() noexcept
    {
        DistributionSummary s;
        s.Count     = totalCount_.exchange(0, std::memory_order_relaxed);
        s.Maximum   = max_.exchange(0, std::memory_order_relaxed);
        s.Average   = s.Count == 0 ? 0 :  sum_.exchange(0, std::memory_order_relaxed) / s.Count;

        auto EstimatePercentile = [&](double q) -> uint64_t
        {
            if (s.Count == 0)
                return 0;

            const auto targetCount = static_cast<uint64_t>(q * s.Count);
            uint64_t cumulativeCount = 0;
            uint64_t lower = 0;

            for (auto i = 0; i < upperBounds_.size(); ++i)
            {
                const uint64_t count = counts_[i].exchange(0, std::memory_order_relaxed);
                //  버킷에 값이 없는 경우
                if (count == 0)
                {
                    lower = upperBounds_[i] + 1;
                    continue;
                }

                //  버킷에 값이 있는 경우
                cumulativeCount += count;
                if (cumulativeCount >= targetCount)
                    return lower;

                lower = upperBounds_[i] + 1;
            }

            return s.Maximum;
        };

        s.P50 = EstimatePercentile(0.50);
        s.P95 = EstimatePercentile(0.95);
        s.P99 = EstimatePercentile(0.99);
        return s;
    }

private:
    std::vector<uint64_t> upperBounds_;
    std::vector<std::atomic<uint64_t>> counts_;
    std::atomic<uint64_t> totalCount_{};
    std::atomic<uint64_t> sum_{};
    std::atomic<uint64_t> max_{};
};

} // namespace psh::lib::telemetry