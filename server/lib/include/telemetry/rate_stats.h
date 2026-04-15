#pragma once

namespace psh::lib::telemetry
{
// 시간당 처리량 스냅샷. 스냅샷 시점 기준 최근 윈도우 평균.

struct RateStats
{
    double SubmittedPerSecond = 0.0;
    double ExecutedPerSecond = 0.0;
    double CompletedPerSecond = 0.0;
    double FailedPerSecond = 0.0;
};

} // namespace psh::lib::telemetry
