#pragma once

#include "job.h"

#include <memory>
#include <string_view>

namespace psh::lib::job
{
class JobQueue;
class WorkerPool;
class TimedJobManager;

// ------------------------------------------------------------
// JobQueueHelper
// ------------------------------------------------------------
// 시간 기반 제출과 반복 실행을 위한 정적 헬퍼.
// TimedJobManager::PostAt(Job, TimePoint)에 대상 제출 로직을 감싼 Job을 전달한다.
//
// JobQueue 대상과 WorkerPool 대상 overload를 모두 제공한다.

struct JobQueueHelper
{
    // --- JobQueue 대상 ---

    // job을 queue->Post로 감싸서 TimedJobManager에 등록한다.
    static bool PostAt(Job job, std::shared_ptr<JobQueue> queue,
                       std::shared_ptr<TimedJobManager> timer, TimePoint at);

    static bool PostAfter(Job job, std::shared_ptr<JobQueue> queue,
                          std::shared_ptr<TimedJobManager> timer, Duration delay);

    static RepeatHandle Repeat(Job job, std::shared_ptr<JobQueue> queue,
                               std::shared_ptr<TimedJobManager> timer, Duration period,
                               EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                               bool executeNow = false);

    // --- WorkerPool 대상 ---

    // job을 pool->Post로 감싸서 TimedJobManager에 등록한다.
    static bool PostAt(Job job, std::shared_ptr<WorkerPool> pool,
                       std::shared_ptr<TimedJobManager> timer, TimePoint at);

    static bool PostAfter(Job job, std::shared_ptr<WorkerPool> pool,
                          std::shared_ptr<TimedJobManager> timer, Duration delay);

    static RepeatHandle Repeat(Job job, std::shared_ptr<WorkerPool> pool,
                               std::shared_ptr<TimedJobManager> timer, Duration period,
                               EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                               bool executeNow = false);

    // --- Repeat 계약 ---
    //   FixedDelay — 실행 완료 시점 + period 후 다음 예약.
    //   FixedRate  — 이전 예약 시점 + period. 실행이 period보다 오래 걸리면
    //               즉시 다음 1회 실행 (catch-up 누적 없음).
    //   Cancel     — 미래 회차 + 이미 예약된 회차 모두 취소.
    //               콜백 실행 중 자기 핸들로 Cancel 호출 가능.
    //   Pause      — 현재 예약된 회차를 취소하고 대기.
    //   Resume     — period를 리셋하여 resume 시점 + period 후 다음 실행.

    // --- 템플릿 버전 (JobQueue 대상) ---

    template <typename Fn>
    static bool PostAt(Fn&& func, std::shared_ptr<JobQueue> queue,
                       std::shared_ptr<TimedJobManager> timer, TimePoint at,
                       std::string_view debugLabel = {})
    {
        return PostAt(Job(std::forward<Fn>(func), debugLabel), std::move(queue), std::move(timer),
                      at);
    }

    template <typename Fn>
    static bool PostAfter(Fn&& func, std::shared_ptr<JobQueue> queue,
                          std::shared_ptr<TimedJobManager> timer, Duration delay,
                          std::string_view debugLabel = {})
    {
        return PostAfter(Job(std::forward<Fn>(func), debugLabel), std::move(queue),
                         std::move(timer), delay);
    }

    template <typename Fn>
    static RepeatHandle Repeat(Fn&& func, std::shared_ptr<JobQueue> queue,
                               std::shared_ptr<TimedJobManager> timer, Duration period,
                               EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                               bool executeNow = false, std::string_view debugLabel = {})
    {
        return Repeat(Job(std::forward<Fn>(func), debugLabel), std::move(queue), std::move(timer),
                      period, mode, executeNow);
    }

    // --- 템플릿 버전 (WorkerPool 대상) ---

    template <typename Fn>
    static bool PostAt(Fn&& func, std::shared_ptr<WorkerPool> pool,
                       std::shared_ptr<TimedJobManager> timer, TimePoint at,
                       std::string_view debugLabel = {})
    {
        return PostAt(Job(std::forward<Fn>(func), debugLabel), std::move(pool), std::move(timer),
                      at);
    }

    template <typename Fn>
    static bool PostAfter(Fn&& func, std::shared_ptr<WorkerPool> pool,
                          std::shared_ptr<TimedJobManager> timer, Duration delay,
                          std::string_view debugLabel = {})
    {
        return PostAfter(Job(std::forward<Fn>(func), debugLabel), std::move(pool), std::move(timer),
                         delay);
    }

    template <typename Fn>
    static RepeatHandle Repeat(Fn&& func, std::shared_ptr<WorkerPool> pool,
                               std::shared_ptr<TimedJobManager> timer, Duration period,
                               EnumRepeatMode mode = EnumRepeatMode::FixedDelay,
                               bool executeNow = false, std::string_view debugLabel = {})
    {
        return Repeat(Job(std::forward<Fn>(func), debugLabel), std::move(pool), std::move(timer),
                      period, mode, executeNow);
    }
};

} // namespace psh::lib::job
