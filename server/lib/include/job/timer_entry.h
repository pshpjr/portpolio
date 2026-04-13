#pragma once

#include "executor.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace psh::lib::job
{
// ------------------------------------------------------------
// TimerEntry — Timer 의 bucket 이 shared_ptr 로 소유.
// JobHandle / RepeatHandle 은 weak_ptr<TimerEntry> 로만 참조한다.
//
// Dispatch: 만료 시 entry 를 대상(Executor 또는 JobQueue) 으로 위임하는 타입 소거된 함수.
//   Timer 생성 시 바인딩된 executor 또는 스케줄 시점에 캡처된 weak<JobQueue> 가
//   이 클로저 내부에 숨겨져 있다. 반환값은 대상이 실제로 Post 를 받아들였는지.
// ------------------------------------------------------------

class TimerEntry
{
  public:
    using DispatchFn =
        std::function<bool(IExecutor::Callback, std::string_view /*label*/)>;

    TimerEntry(EntryId id,
               std::string_view debugLabel,
               IExecutor::Callback fn,
               TimePoint scheduledAt,
               uint64_t bucket,
               DispatchFn dispatch)
        : Id(id)
        , DebugLabel(debugLabel)
        , Fn(std::move(fn))
        , ScheduledAt(scheduledAt)
        , Bucket(bucket)
        , Dispatch(std::move(dispatch))
    {
    }

    const EntryId Id;
    const std::string DebugLabel;

    IExecutor::Callback Fn;
    TimePoint ScheduledAt;
    uint64_t Bucket;
    DispatchFn Dispatch;

    std::atomic<EnumJobState> State{EnumJobState::WaitingTimer};
    std::atomic<bool> Canceled{false};

    // Repeat 전용 (1회성 엔트리에서는 Period 가 nullopt)
    std::optional<Duration> Period;
    std::atomic<Duration::rep> PeriodNs{0};
    std::atomic<EnumRepeatMode> Mode{EnumRepeatMode::FixedDelay};
    std::atomic<bool> Paused{false};
};

} // namespace psh::lib::job
