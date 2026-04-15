# Exec Plan: 범용 Telemetry Recorder

> 생성: 2026-04-15
> 영역: `server/lib/include/telemetry/`, `server/lib/include/job/`
> 관련: [`20260413-job-system-redesign.md`](./20260413-job-system-redesign.md) D21

## 목표

`psh::lib::telemetry` 의 스냅샷 타입(`HistogramSnapshot` / `LatencySummary` / `RateStats`) 만 존재하고 **기록(record)·집계(aggregate) 측이 비어 있던 상태**를 메운다. 도메인(latency / job 통계)에 묶이지 않는 범용 Recorder 를 도입하고, JobQueue 가 wait/exec 시간을 실제로 채우도록 hook 한다.

## 배경 (현재 문제)

1. `JobQueue::GetStatsSnapshot()` 이 `Rates` / `WaitTime` / `ExecutionTime` / `WaitHistogram` / `ExecutionHistogram` 필드를 **기본값 그대로 반환**. (`server/lib/include/job/job_queue.h:179`)
2. `ExecuteEntry()` 가 wait / execution duration 을 측정하지 않음.
3. `histogram.h` 가 `Duration` 에 묶여 있어 latency 외 도메인(byte/queue depth) 재사용 불가.
4. `RateStats` 가 `SubmittedPerSecond` / `ExecutedPerSecond` 등 **job 도메인 어휘**로 채워져 있어 다른 호출자가 못 씀.
5. `job_queue.h:5` include 라인 인코딩 깨짐 (`#include "job_?쀫Т??h"`).

## 설계 결정

| # | 결정 |
|---|---|
| T1 | Recorder 는 값 단위 무관. **`uint64_t` 단일** 값 입력. 단위(ns/bytes/depth) 해석은 호출측 책임 |
| T2 | 1차 구현은 **lock-free 원자 버킷 카운터**. `IsApproximate=false`. 카운트는 lossless |
| T3 | hot path 부하가 커질 경우 **per-thread shard + merge** 로 교체 가능. 인터페이스(`Record` / `Snapshot`) 불변. 헤더 주석으로 명시 |
| T4 | 라이브러리는 INTERFACE(헤더 온리). 모든 Recorder 도 헤더 온리로 둠 |
| T5 | `LatencySummary` 는 `DistributionSummary` 로 일반화. **percentile 은 Histogram 스냅샷에서 보간 계산** (`Summarize(snapshot)` 자유 함수) |
| T6 | `RateStats` 4-필드 묶음 제거. `RateSnapshot { PerSecond, Total }` + `RateMeter`. 호출측이 필요한 만큼 여러 RateMeter 를 따로 보유 |
| T7 | `Histogram` 은 `sum` / `max` / `count` 를 부수 atomic 으로 들고, 평균/최댓값을 lossless 로 노출 |

## 범위

### 새로 생성
- `server/lib/include/telemetry/distribution.h` — `DistributionSummary` + `Summarize(HistogramSnapshot)`
- `server/lib/include/telemetry/rate.h` — `RateMeter`, `RateSnapshot`

### 수정
- `server/lib/include/telemetry/histogram.h` — Duration 제거, `Histogram` 클래스 추가, `HistogramSnapshot` 에 Sum/Max/TotalCount 필드 추가
- `server/lib/include/job/job_queue_types.h` — 스냅샷 필드를 새 타입으로 교체 (`LatencySummary` → `DistributionSummary`, `RateStats` → 개별 `RateSnapshot` 멤버들)
- `server/lib/include/job/job_queue.h` — recorder 멤버 추가, `Post`/`ExecuteEntry` 에서 측정·기록, `GetStatsSnapshot` 에서 스냅샷 합성. 깨진 include 복구
- `server/lib/include/job/README.md` — 관측 섹션 갱신

### 제거
- `server/lib/include/telemetry/latency_summary.h`
- `server/lib/include/telemetry/rate_stats.h`

### 범위 외
- WorkerPool / Timer 의 hook (별도 step. 본 plan 은 JobQueue 만)
- 단위 테스트 추가 (현 단계는 골격 + 최소 hook. 테스트는 후속)
- HDR / per-thread shard 구현 (T3 의 미래 옵션. 본 plan 은 인터페이스 보존만 약속)

## 단계

- [x] 0. exec-plan 작성 + INDEX 갱신
- [x] 1. `histogram.h` 재작성 (Recorder + Snapshot 확장 + DistributionSummary/Summarize 통합)
- [x] 2. `rate.h` 신설, 구 헤더(`latency_summary.h`, `rate_stats.h`) 제거
- [x] 3. `job_queue_types.h` 스냅샷 갱신
- [x] 4. `JobQueue` recorder hook (wait/exec histogram + 3 RateMeter)
- [x] 5. `job/README.md` 관측 섹션 갱신

## 변경 요약

- `distribution.h` 는 별도 파일 분리하지 않고 `histogram.h` 안에 `DistributionSummary` + `Summarize()` 합침 (사용처가 항상 Histogram 과 묶임).
- `JobQueue` 기본 버킷: 1µs / 10µs / 100µs / 1ms / 10ms / 100ms / 1s / 10s + sentinel.
- `job_queue.h:5` 깨진 include 는 작업 시점에 이미 복구돼 있어 별도 수정 없음.

## 완료 기준

- `JobQueue::GetStatsSnapshot()` 결과의 `WaitTime.Average` / `ExecutionHistogram.Buckets` 등이 실제 측정값으로 채워진다.
- `psh::lib::telemetry` 가 latency 어휘 없이 일반 Recorder 로만 구성된다.
- `histogram.h` 헤더 상단 주석에 "1차는 lock-free 원자 버킷, 부하 커지면 per-thread shard 로 교체 가능 (인터페이스 불변)" 명시가 있다.
- `job_queue.h` 가 깨진 include 없이 컴파일된다.