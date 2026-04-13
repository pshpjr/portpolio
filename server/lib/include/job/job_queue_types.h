#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace psh::lib::job
{
using ClockDuration = std::chrono::nanoseconds;
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::nanoseconds;

using EntryId = uint64_t;   // 컴포넌트(JobQueue / Timer) 별 단조 증가 id. 디버그/로그 용도.

// ------------------------------------------------------------
// Enums
// ------------------------------------------------------------

enum class EnumJobState : uint8_t
{
    WaitingTimer,
    Queued,
    Executing,
    Done,
    Failed,
    Canceled,
    Rejected
};

enum class EnumJobQueueState : uint8_t
{
    Running,
    Draining,
    Stopped
};

enum class EnumRepeatMode : uint8_t
{
    FixedRate,
    FixedDelay
};

enum class EnumCancelResult : uint8_t
{
    Canceled,
    AlreadyCanceled,
    AlreadyExecuting,
    AlreadyFinished,
    Expired
};

// ------------------------------------------------------------
// Common Option / Observation Types
// ------------------------------------------------------------

// 느릴 때 로그 찍는 기준
struct SlowJobOptions
{
    Duration WarningThreshold = std::chrono::milliseconds(0);
    bool EnableWarningLog = false;
};

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

struct RateStats
{
    double SubmittedPerSecond = 0.0;
    double ExecutedPerSecond = 0.0;
    double CompletedPerSecond = 0.0;
    double FailedPerSecond = 0.0;
};

struct LatencySummary
{
    Duration Average{};
    Duration Maximum{};
    Duration P50{};
    Duration P95{};
    Duration P99{};
};

// 관측 API 단일 진입점: 모든 지표는 Get*StatsSnapshot() 하나로 노출.
struct JobQueueStatsSnapshot
{
    EnumJobQueueState State = EnumJobQueueState::Stopped;

    uint64_t PendingCount = 0;
    uint64_t SubmittedCount = 0;
    uint64_t ExecutedCount = 0;
    uint64_t FailedCount = 0;
    uint64_t RejectedCount = 0;
    uint64_t CanceledCount = 0;
    uint64_t BatchLimitHitCount = 0;
    uint64_t DrainRescheduleCount = 0;

    RateStats Rates{};
    LatencySummary WaitTime{};
    LatencySummary ExecutionTime{};
    HistogramSnapshot WaitHistogram{};
    HistogramSnapshot ExecutionHistogram{};
};

struct WorkerStateSnapshot
{
    uint32_t ThreadIndex = 0;
    bool IsRunning = false;
    bool IsIdle = false;
    std::optional<std::string> CurrentLabel;
};

struct WorkerPoolStatsSnapshot
{
    EnumJobQueueState State = EnumJobQueueState::Stopped;

    uint32_t WorkerCount = 0;
    uint64_t PendingCount = 0;
    uint64_t SubmittedCount = 0;
    uint64_t ExecutedCount = 0;
    uint64_t FailedCount = 0;
    uint64_t RejectedCount = 0;

    RateStats Rates{};
    LatencySummary QueueWaitTime{};
    LatencySummary ExecutionTime{};
    HistogramSnapshot QueueWaitHistogram{};
    HistogramSnapshot ExecutionHistogram{};

    std::vector<WorkerStateSnapshot> Threads;
};

} // namespace psh::lib::job
