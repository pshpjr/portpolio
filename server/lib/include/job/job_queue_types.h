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

// ------------------------------------------------------------
// Enums
// ------------------------------------------------------------

enum class EnumJobState : uint8_t
{
    WaitingTimer,
    Queued,
    Executing,
    Done,
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

// 주석 메모:
// 현재 단계에서는 외부 계약상 job cancel은 적극 지원하지 않는 방향으로 단순화.
// 필요 시 추후 확장.
enum class EnumCancelResult : uint8_t
{
    Canceled,
    AlreadyExecuting,
    AlreadyFinished,
    NotFound,
    Rejected
};

// ------------------------------------------------------------
// Common Option Types
// ------------------------------------------------------------

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

struct JobQueueStatsSnapshot
{
    EnumJobQueueState State = EnumJobQueueState::Stopped;

    uint64_t PendingCount = 0;
    uint64_t TimedWaitingCount = 0;
    bool IsExecuting = false;

    uint64_t SubmittedCount = 0;
    uint64_t ExecutedCount = 0;
    uint64_t CompletedCount = 0;
    uint64_t FailedCount = 0;
    uint64_t RejectedCount = 0;

    uint64_t BatchLimitHitCount = 0;
    uint64_t DrainRescheduleCount = 0;

    LatencySummary WaitTime{};
    LatencySummary ExecutionTime{};
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
    uint32_t WorkerCount = 0;

    uint64_t SubmittedCount = 0;
    uint64_t ExecutedCount = 0;
    uint64_t CompletedCount = 0;
    uint64_t FailedCount = 0;
    uint64_t RejectedCount = 0;

    LatencySummary QueueWaitTime{};
    LatencySummary ExecutionTime{};

    std::vector<WorkerStateSnapshot> Threads;
};

} // namespace psh::lib::job
