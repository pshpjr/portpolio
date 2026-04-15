#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

namespace psh::lib::telemetry
{
// 범용 히스토그램 스냅샷. Duration 기반 버킷으로 latency/wait/execution 등 공통 사용.
// IsApproximate=true 인 구현(HDR / lock-free accumulator)은 merge 시점 오차를 허용한다.

using Duration = std::chrono::nanoseconds;

struct HistogramBucket
{
    Duration LowerBound{};
    Duration UpperBound{};
    uint64_t Count = 0;
};

struct HistogramSnapshot
{
    std::vector<HistogramBucket> Buckets;
    bool IsApproximate = true;
};

} // namespace psh::lib::telemetry
