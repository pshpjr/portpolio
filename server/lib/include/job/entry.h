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
// Entry — JobQueue / Timer 가 공유하는 단일 엔트리 타입.
//
// 두 경로에서 shared_ptr<Entry> 로 소유되고, Handle(JobHandle / RepeatHandle) 은
// weak_ptr<Entry> 로만 참조한다. 경로 구분은 내부 Kind 로만 하며 외부 노출은 없다.
//
//   A) JobQueue::Post        — 즉시 실행 대상.
//                              ScheduledAt = 제출 시각.
//                              Dispatch / Bucket / Period 는 사용하지 않는다.
//
//   B) Timer::Schedule*      — 시각 도달 후 Dispatch 로 대상(Executor / JobQueue)
//                              에 재위임. Period 가 있으면 Repeat.
//
// Cancel 은 Canceled atomic flip 만으로 처리(lazy cancel). 실행 직전에 관측하여
// skip 한다. bucket/queue 에서의 erase 는 수행하지 않는다.
// ------------------------------------------------------------

class Entry
{
  private:
    // 엔트리 경로 태그. 외부 계약이 아니라 구현 내 assertion/debug 용도.
    enum class EnumKind : uint8_t
    {
        Plain,  // JobQueue::Post 로 생성. 즉시 실행.
        Timer   // Timer::Schedule* 로 생성. 시각 도달 후 Dispatch.
    };

  public:
    using DispatchFn =
        std::function<bool(IExecutor::Callback, std::string_view /*label*/)>;

    // JobQueue 경로용 생성자. 초기 상태 Queued.
    Entry(EntryId id,
          std::string_view debugLabel,
          IExecutor::Callback fn,
          TimePoint scheduledAt)
        : Id(id)
        , DebugLabel(debugLabel)
        , Fn(std::move(fn))
        , ScheduledAt(scheduledAt)
        , kind_(EnumKind::Plain)
    {
    }

    // Timer 경로용 생성자. 초기 상태 WaitingTimer.
    Entry(EntryId id,
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
        , kind_(EnumKind::Timer)
    {
        State.store(EnumJobState::WaitingTimer, std::memory_order_relaxed);
    }

    const EntryId Id;
    const std::string DebugLabel;

    IExecutor::Callback Fn;

    // JobQueue: 제출 시각. Timer: 만료(예정) 시각. Repeat 재예약 시 갱신.
    TimePoint ScheduledAt;

    // Timer 영역
    uint64_t Bucket = 0; //버킷 키. Repeat 재예약 시 갱신. JobQueue 에서는 사용 안 함.
    DispatchFn Dispatch;

    std::atomic<EnumJobState> State{EnumJobState::Queued};
    std::atomic<bool> Canceled{false};

    // Repeat 영역
    std::optional<Duration> Period; // nullopt 이면 1회성. Timer 에서만 세팅된다.
    std::atomic<Duration::rep> PeriodNs{0};
    std::atomic<EnumRepeatMode> Mode{EnumRepeatMode::FixedDelay};
    std::atomic<bool> Paused{false};

  private:
    const EnumKind kind_;
};

} // namespace psh::lib::job
