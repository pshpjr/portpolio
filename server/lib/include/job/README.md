# Job System

`psh::lib::job` — Executor / JobQueue / Timer 3축으로 구성된 비동기 실행 라이브러리.

> 상세 인터페이스 계약: [`server/docs/exec-plans/active/20260413-job-system-interface-spec.md`](../../../docs/exec-plans/active/20260413-job-system-interface-spec.md)

## 1. 구성

| 컴포넌트 | 역할 | 파일 |
|---|---|---|
| `IExecutor` / `ThreadPoolExecutor` | 함수 실행 엔진 (스레드풀 추상). 단일 API `Post(Callback)` (실패 없음). | `executor.h`, `thread_pool_executor.h` |
| `JobQueue` | Executor 위에 쌓이는 **직렬 실행 큐**. 자체 저장소. | `job_queue.h` |
| `Timer` | 시각 기반 스케줄러. 만료 시 Executor / JobQueue 로 재위임. | `timer.h` |
| `Entry` | 두 경로(JobQueue / Timer) 가 공유하는 단일 엔트리 타입. | `entry.h` |
| `JobHandle` / `RepeatHandle` | Entry 를 weak 참조. 취소·일시정지·설정 변경. | `job.h` |
| 공용 타입 / 통계 | enum, snapshot 구조체. | `job_queue_types.h` |

계측 공용 타입·Recorder(`Histogram` / `RateMeter` / `DistributionSummary`)는 `psh::lib::telemetry` (`server/lib/include/telemetry/`). 도메인 무관 — JobQueue 외 다른 컴포넌트(network/io 등)도 동일 Recorder 를 직접 사용한다.

## 2. 핵심 모델

### 2.1 단일 스케줄링 프리미티브

`IExecutor::Post(Callback)` 하나가 전부. 실패하지 않는다 (종료 상태에서 호출하면 fn 은 조용히 소멸).

```cpp
auto pool = std::make_shared<ThreadPoolExecutor>(
    ThreadPoolExecutor::CreateOptions{ .WorkerCount = 4, .DebugName = "main-pool" });

pool->Post([] { /* run on a worker */ });
```

### 2.2 JobQueue (직렬 실행)

`JobQueue` 는 Executor 위에 얹히는 **strand-like** 컨테이너. 자기 drain 클로저를 `executor->Post` 로 넘겨 실행 슬롯을 확보한다. 직렬성은 `running_` 플래그 + `mtx_` 로 보장 — 동일 JobQueue 의 두 잡이 동시에 실행되지 않는다.

```cpp
auto session = std::make_shared<JobQueue>(pool);
session->Post([] { /* runs serially per session */ });
session->Post([] { /* runs after the previous one completes */ });
```

`BatchLimit` 소진 시 self-requeue 로 starvation 방지.

### 2.3 Timer

전용 watcher 스레드 1개 + 버킷 자료구조 (`unordered_map<bucket, list<Entry>> + set<bucket>`). **watcher 는 절대 사용자 콜백을 직접 실행하지 않는다** — 만료 시 항상 Executor 또는 JobQueue 로 재위임.

```cpp
auto timer = std::make_shared<Timer>(pool);
timer->Start();

// Executor 대상
auto h1 = timer->ScheduleAfter(std::chrono::seconds(1), [] { /* on pool worker */ });

// JobQueue 대상 — session 의 직렬성 안에서 실행됨
auto h2 = timer->ScheduleAfter(session, std::chrono::seconds(2), [] { /* serial */ });

// 반복
auto rep = timer->ScheduleRepeat(session, std::chrono::milliseconds(100),
                                 [] { /* every 100ms */ },
                                 EnumRepeatMode::FixedDelay);
```

### 2.4 Entry / Handle 패턴

- **Entry** 는 JobQueue / Timer 컨테이너가 `shared_ptr` 로 소유한다.
- 외부에는 `weak_ptr<Entry>` 를 감싼 `JobHandle` / `RepeatHandle` 만 노출.
- `Cancel()` 은 atomic 플래그를 flip 하는 lazy cancel — bucket / queue 에서 즉시 제거하지 않고 실행 직전에 관측해 skip 한다.
- 두 경로는 한 클래스에서 처리하지만 사용자에게는 보이지 않는다 (private `EnumKind`).

## 3. 사용 흐름

### 3.1 게임 세션 (전형 예)

```cpp
auto pool   = std::make_shared<ThreadPoolExecutor>(/*opts*/);
auto timer  = std::make_shared<Timer>(pool); timer->Start();
auto sess   = std::make_shared<JobQueue>(pool);

// 들어온 패킷을 세션 큐에 직렬 처리
sess->Post([packet] { handle(packet); });

// 30초 후 idle disconnect 검사 — 같은 세션 큐에서 실행
auto timeout = timer->ScheduleAfter(sess, 30s,
    [w = std::weak_ptr(sess)] {
        if (auto s = w.lock()) s->Post([] { check_idle(); });
    });

// 패킷 수신 시 갱신
timeout.Cancel();
```

### 3.2 종료 (graceful)

```cpp
timer->Stop();                       // watcher 정지, 잔여 엔트리 폐기
sess->Stop(/*drain=*/true, 5s);     // 큐 비울 때까지 대기 (timeout 5s)
pool->Stop(/*drain=*/true, 5s);     // 워커 join
```

순서는 **위에서 아래로** 권장: 새 스케줄을 만들어내는 쪽부터 끈다.

## 4. 관측

각 컴포넌트는 단일 진입점 `GetStatsSnapshot()` 만 노출.

```cpp
auto s = sess->GetStatsSnapshot();
// s.PendingCount, s.ExecutedCount, s.Rates, s.WaitTime, s.WaitHistogram, ...
```

스냅샷 구조는 `job_queue_types.h` 의 `JobQueueStatsSnapshot` / `WorkerPoolStatsSnapshot` 참조.

## 5. 테스트 지원

- `JobQueue::Pump()` / `Pump(n)` / `Flush()` — 호출 스레드에서 직접 drain. 단위 테스트용.
- `Timer::CreateOptions::NowProvider` — FakeClock 람다 주입으로 결정적 테스트 가능.

## 6. 주의

- `JobQueue` / `Timer` / `ThreadPoolExecutor` 는 모두 `enable_shared_from_this`. 반드시 `std::make_shared<T>(...)` 로 생성한다.
- Timer 가 JobQueue 를 대상으로 잡을 때는 `weak_ptr<JobQueue>` 만 보유. 대상 소멸 시 만료 회차는 조용히 drop + debug log.
- `Stop(drain=true, timeout)` 에서 timeout 초과 시 잔여 잡은 폐기되고 카운터(`droppedOnStopCount_` / `canceledCount_`) 가 증가한다.
