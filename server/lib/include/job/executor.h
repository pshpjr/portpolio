#pragma once

#include "job_queue_types.h"

#include <functional>

namespace psh::lib::job
{
class IExecutor
{
public:
    using Callback = std::function<void()>;

    virtual ~IExecutor() = default;

    // Post 는 실패하지 않는다. 종료 상태에서 호출하면 fn 은 호출되지 않고 조용히 소멸.
    virtual void Post(Callback fn) = 0;

    virtual EnumJobQueueState GetState() const noexcept = 0;

    // Running → Draining → Stopped.
    // drain=true  : 기존 큐 소진 대기. drainTimeout 초과 시 폐기 + 경고.
    // drain=false : 큐 즉시 clear.
    virtual void Stop(bool drain = true, Duration drainTimeout = Duration::zero()) = 0;

    virtual WorkerPoolStatsSnapshot GetStatsSnapshot() const = 0;
};

} // namespace psh::lib::job
