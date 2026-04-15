#pragma once

#include "telemetry/histogram.h"
#include "telemetry/rate.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace psh::lib::job
{
using HistogramBucket = telemetry::HistogramBucket;
using HistogramSnapshot = telemetry::HistogramSnapshot;
using DistributionSummary = telemetry::DistributionSummary;
using RateSnapshot = telemetry::RateSnapshot;

using ClockDuration = std::chrono::nanoseconds;
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::nanoseconds;

using EntryId = uint64_t; // 컴포넌트(JobQueue / Timer) 별 단조 증가 id. 디버그/로그 용도.

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
    Rejected,
    Expired, // 핸들이 weak_ptr 로만 보유 — 컨테이너에서 entry 가 이미 해제됨 (관측 불가)
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

// 관측 API 단일 진입점: 모든 지표는 Get*StatsSnapshot() 하나로 노출.
struct JobQueueStatsSnapshot
{
    EnumJobQueueState State = EnumJobQueueState::Stopped;

    uint64_t PendingCount = 0;
    uint64_t SubmittedCount = 0;
    uint64_t ExecutedCount = 0;
    uint64_t FailedCount = 0;
    uint64_t CanceledCount = 0;
    uint64_t BatchLimitHitCount = 0;
    uint64_t DrainRescheduleCount = 0;

    RateSnapshot SubmittedRate{};
    RateSnapshot ExecutedRate{};
    RateSnapshot FailedRate{};

    HistogramSnapshot WaitHistogram{};      // ns
    HistogramSnapshot ExecutionHistogram{}; // ns
    DistributionSummary WaitTime{};         // ns
    DistributionSummary ExecutionTime{};    // ns
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

    RateSnapshot SubmittedRate{};
    RateSnapshot ExecutedRate{};
    RateSnapshot FailedRate{};

    HistogramSnapshot QueueWaitHistogram{}; // ns
    HistogramSnapshot ExecutionHistogram{}; // ns
    DistributionSummary QueueWaitTime{};    // ns
    DistributionSummary ExecutionTime{};    // ns

    std::vector<WorkerStateSnapshot> Threads;
};

} // namespace psh::lib::job
