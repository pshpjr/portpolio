#pragma once

#include "executor.h"

#include <atomic>
#include <string>
#include <string_view>
#include <utility>

namespace psh::lib::job
{
// ------------------------------------------------------------
// JobEntry — JobQueue 가 shared_ptr 로 소유하는 개별 Job 상태.
// JobHandle 은 weak_ptr<JobEntry> 로만 참조한다.
// Cancel 은 Canceled atomic 플립. DrainOnce 가 관측하여 skip.
// ------------------------------------------------------------

class JobEntry
{
  public:
    JobEntry(EntryId id,
             std::string_view debugLabel,
             TimePoint submittedAt,
             IExecutor::Callback fn)
        : Id(id)
        , DebugLabel(debugLabel)
        , SubmittedAt(submittedAt)
        , Fn(std::move(fn))
    {
    }

    const EntryId Id;
    const std::string DebugLabel;
    const TimePoint SubmittedAt;
    IExecutor::Callback Fn;

    std::atomic<EnumJobState> State{EnumJobState::Queued};
    std::atomic<bool> Canceled{false};
};

} // namespace psh::lib::job
