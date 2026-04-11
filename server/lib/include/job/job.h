#pragma once

#include "job_queue_types.h"

#include <functional>
#include <memory>
#include <string_view>

namespace psh::lib::job
{
// ------------------------------------------------------------
// Job / Handles
// ------------------------------------------------------------

class Job
{
  public:
    using Callback = std::function<void()>;

  public:
    Job() = default;

    // Job의 경우 외부에서 모르는 타입이므로, 암시적으로 생성 가능하게 한다.
    // 호출 가능 객체로부터 직접 생성.
    Job(Callback callback, std::string_view debugLabel = {})
        : callback_(std::move(callback)), debugLabel_(debugLabel)
    {
    }

    template <typename Fn>
        requires std::invocable<Fn> && (!std::same_as<std::decay_t<Fn>, Job>)
    Job(Fn&& func, std::string_view debugLabel = {})
        : callback_(std::forward<Fn>(func)), debugLabel_(debugLabel)
    {
    }

    // TODO: 멤버 strong bind / weak bind는 고민 중
    // owner를 weak 캡쳐 후 없으면 사라지게 할지...
    template <typename T, typename Ret, typename... Args>
    Job(std::shared_ptr<T> owner, Ret (T::*memFunc)(Args...), Args&&... args)
        : callback_(
              [o = std::move(owner), memFunc, ... a = std::forward<Args>(args)]() mutable
              {
                  (o.get()->*memFunc)(std::forward<Args>(a)...);
              })
    {
    }

    template <typename T, typename Ret, typename... Args>
    Job(std::shared_ptr<T> owner, Ret (T::*memFunc)(Args...) const, Args&&... args)
        : callback_(
              [o = std::move(owner), memFunc, ... a = std::forward<Args>(args)]() mutable
              {
                  (o.get()->*memFunc)(std::forward<Args>(a)...);
              })
    {
    }

  public:
    [[nodiscard]] bool IsValid() const noexcept
    {
        return static_cast<bool>(callback_);
    }

    [[nodiscard]] std::string_view GetDebugLabel() const noexcept
    {
        return debugLabel_;
    }

    void operator()() const
    {
        if (callback_)
            callback_();
    }

  private:
    Callback callback_;
    std::string debugLabel_;
};

class JobHandle
{
  public:
    JobHandle() = default;

    bool IsValid() const noexcept;
    bool Expired() const noexcept;

    // 주석 메모:
    // 현재 단계에서는 외부 계약상 job cancel을 단순화.
    // 필요 시 추후 활성화 가능.
    EnumCancelResult Cancel();

    EnumJobState TryGetState() const;
    std::string GetDebugLabel() const;
};

// ------------------------------------------------------------
// RepeatHandle — 반복 실행 제어 핸들
// ------------------------------------------------------------
// 계약:
//   FixedRate  — 실행이 period보다 오래 걸리면 즉시 다음 1회 실행 (catch-up 없음).
//   FixedDelay — 실행 완료 시점 + period 후 다음 실행.
//   Pause      — 현재 예약된 회차를 취소하고 대기 상태로 전환.
//   Resume     — period를 리셋하여 resume 시점 + period 후 다음 실행.
//   Cancel     — 미래 회차 생성 중단 + 이미 예약된 회차도 취소.
//                콜백 실행 중 자기 자신의 핸들로 Cancel 호출 가능.

class RepeatHandle
{
  public:
    RepeatHandle() = default;

    bool IsValid() const noexcept;
    bool Expired() const noexcept;

    bool Cancel();
    bool Pause();
    bool Resume();

    bool ChangePeriod(Duration newPeriod);
    bool ChangeMode(EnumRepeatMode newMode);

    std::optional<EnumRepeatMode> TryGetMode() const;
    std::optional<Duration> TryGetPeriod() const;
    std::string GetDebugLabel() const;
};

} // namespace psh::lib::job
