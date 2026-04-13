#pragma once

#include <chrono>

namespace psh::lib::telemetry
{
// 지연 분포 요약. Duration 기반.
// 백분위는 스냅샷 시점에서 누적 표본으로 계산한 값 (구현체에 따라 근사일 수 있음).

using Duration = std::chrono::nanoseconds;

struct LatencySummary
{
    Duration Average{};
    Duration Maximum{};
    Duration P50{};
    Duration P95{};
    Duration P99{};
};

} // namespace psh::lib::telemetry
