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

    // 정상 true. 포화/종료 상태 false. 실패 시 fn 은 호출되지 않고 소멸.
    virtual bool Post(Callback fn) = 0;

    virtual EnumJobQueueState GetState() const noexcept = 0;

    // Running → Draining → Stopped.
    // drain=true  : 기존 큐 소진 대기. drainTimeout 초과 시 폐기 + 경고.
    // drain=false : 큐 즉시 clear.
    virtual void Stop(bool drain = true, Duration drainTimeout = Duration::zero()) = 0;

    virtual WorkerPoolStatsSnapshot GetStatsSnapshot() const = 0;
};

} // namespace psh::lib::job
