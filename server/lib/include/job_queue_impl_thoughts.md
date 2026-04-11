좋습니다. 반영해서 문서형으로 다시 다듬었습니다.
이번에는 특히 아래 3가지를 넣었습니다.

* **일단 주석 수준에서는 “job 자체는 cancel 안 됨”으로 단순화**
* **job도 `PushAfter` 같은 예약 실행 대상이 될 수 있음**
* **strong bind / weak bind는 아직 결정 전 논의 항목으로 주석성 메모 추가**
* **외부 API는 단순화**

아래 버전으로 붙여넣으시면 됩니다.

---

# 게임 서버 JobQueue / TimedJobManager / ThreadPool 기능 요구사항 초안 v3

## 1. 개요

본 문서는 게임 서버에서 사용하는 비동기 작업 실행 계층의 기능 요구사항을 정의한다.

대상 구성 요소는 다음과 같다.

* `ThreadPool`
* `JobQueue`
* `TimedJobManager`
* `Job`

본 시스템은 다음 목적을 가진다.

* 동일 실행 그룹 내 작업의 **직렬 실행 보장**
* 서로 다른 실행 그룹 간 **병렬 처리**
* 예약 실행 및 반복 실행 지원
* 종료 시 안전한 draining
* 테스트, 디버깅, 관측 기능 제공

---

## 2. 구성 요소와 책임

### 2.1 ThreadPool

`ThreadPool`은 worker thread 집합과 task 실행 기반을 제공한다.

책임은 다음과 같다.

* worker thread 생성 및 관리
* 전달된 실행 단위(task)의 병렬 실행
* 현재 실행 중인 worker 상태 및 통계 제공

`ThreadPool`은 개별 job의 순서 보장, 예약 시간 관리, 반복 실행 정책을 직접 담당하지 않는다.

즉, `ThreadPool`은 **low-level executor** 역할에 집중한다.

또한 `ThreadPool`은 단일 job을 직접 `push / post`할 수 있어야 한다.
단, 이 경로는 queue 기반 순서 보장 없이 동작하는 일반 병렬 실행 경로로 본다.

---

### 2.2 JobQueue

`JobQueue`는 strand와 유사한 실행 단위다.

책임은 다음과 같다.

* 동일 `JobQueue` 내부 job의 **동시 실행 금지**
* 동일 `JobQueue` 내부 ready job의 **직렬 실행 보장**
* timed job이 ready 상태가 되었을 때의 실행 담당
* drain ownership 관리
* 상태 및 통계 관리
* 종료 시 draining 처리

`JobQueue`는 thread를 직접 소유하지 않고, 실행이 필요할 때 `ThreadPool`에 drain task를 등록하는 방식으로 동작한다.

즉, `ThreadPool`에는 개별 job뿐 아니라 `JobQueue`의 drain 실행 단위도 들어갈 수 있다.

---

### 2.3 TimedJobManager

`TimedJobManager`는 시간 기반 예약 실행을 관리한다.

책임은 다음과 같다.

* `PushAt`
* `PushAfter`
* `Repeat`

에 해당하는 예약 정보 관리

* 시간이 도달한 job을 ready 상태로 전환
* 필요 시 해당 `JobQueue`에 승격
* repeat 정책에 따라 다음 실행 시점 재등록
* 테스트용 시간 소스 주입 지원

`TimedJobManager`는 실제 실행을 담당하지 않는다.
실제 실행은 `ThreadPool` 직접 실행 또는 `JobQueue` 경유 실행으로 이루어진다.

---

### 2.4 Job

`Job`은 실행 단위를 나타내는 내부 객체다.

`Job`은 최소한 다음 정보를 가질 수 있어야 한다.

* 상태 enum
* 실행 function
* 생성 시각 / enqueue 시각 / 시작 시각
* 예약 실행 정보
* repeat 정보
* 통계용 메타데이터

외부 API는 callable 기반으로 단순하게 유지하되, 내부적으로는 `Job` 객체를 생성하여 관리한다.

---

## 3. 실행 방식

시스템은 다음 실행 방식을 지원해야 한다.

* `Post`
* `Push`
* `PushAt`
* `PushAfter`
* `Repeat(period, executeNow?, mode)`

여기서:

* `Post`: 호출 문맥에서 즉시 실행될 수도 있는 실행 방식
* `Push`: 반드시 비동기적으로 다른 실행 흐름에서 처리되는 방식
* `PushAt`: 절대 시각 기반 예약 실행
* `PushAfter`: 상대 시간 기반 예약 실행
* `Repeat`: 반복 실행 등록

---

## 4. Post / Push 의미

### 4.1 Post

* 구현 정책에 따라 호출 스레드에서 즉시 실행될 수 있다.
* 다만 필요 시 executor에 등록되어 비동기 실행될 수도 있다.

### 4.2 Push

* 반드시 비동기적으로 등록되어야 한다.
* 호출 스레드에서 직접 실행되면 안 된다.
* 즉, 실행은 항상 다른 실행 흐름에서 일어나야 한다.

---

## 5. 실행 모델

### 5.1 JobQueue는 strand 유사 보장 단위다

* 동일 `JobQueue` 내 job은 동시에 실행되어서는 안 된다.
* 동일 `JobQueue` 내 ready 상태 job은 순차적으로 실행되어야 한다.
* 서로 다른 `JobQueue`는 `ThreadPool` 상에서 병렬 실행될 수 있다.

---

### 5.2 ThreadPool 직접 실행 허용

`ThreadPool`은 단일 job을 직접 `Post / Push / PushAfter / PushAt`할 수 있어야 한다.

즉, job은 반드시 `JobQueue`를 거쳐야만 하는 것은 아니다.

다만 이 경우:

* queue 기반 직렬 실행 보장 없음
* queue draining semantics 없음
* 특정 queue에 귀속된 통계/상태 추적 없음

으로 취급한다.

즉, `ThreadPool` 직접 실행은 **unordered parallel execution 경로**다.

---

### 5.3 JobQueue 경유 실행

`JobQueue`에 등록된 job은 개별 job을 직접 worker에 붙이는 것이 아니라,
queue의 drain 실행을 `ThreadPool`에 올리는 방식으로 처리한다.

즉:

* ready job enqueue
* drainer 부재 시 drain task 등록
* drain task가 queue에서 순차 실행

의 구조를 가진다.

---

## 6. 시간 정밀도

* job 실행 시점의 최소 정밀도는 서버 전역 시계의 최소 단위를 따른다.
* 기준 시계는 서버 전역에서 사용하는 타이머를 기준으로 한다.
* 현재 가정상 **언리얼의 FApp 타이머 수준**을 사용한다.
* 따라서 더 높은 별도 정밀도는 요구하지 않는다.
* 예약 실행은 전역 시계 정밀도 범위 내에서 동작하면 된다.

---

## 7. Job 모델

## 7.1 내부 상태

`Job`은 최소한 다음 상태를 가질 수 있어야 한다.

* `WaitingTimer`
* `Queued`
* `Executing`
* `Done`
* `Canceled`
* `Rejected`

---

## 7.2 내부 구성

`Job`은 최소한 다음 내부 멤버를 가질 수 있어야 한다.

* 상태 enum
* 실행 function (`std::function<void()>` 등)
* 시간 계측용 메타데이터
* repeat / timed 메타데이터
* owner / queue 관련 참조 정보

---

## 7.3 외부 API 단순화

외부 사용자는 `Job` 객체를 직접 만들지 않아도 되도록 해야 한다.

즉 외부 API는 다음처럼 callable 기반으로 단순해야 한다.

```cpp id="ftkz2q"
queue->Push([] { ... });
queue->PushAfter(100ms, [] { ... });
threadPool->Push([] { ... });
threadPool->PushAfter(1s, [] { ... });
```

내부적으로는 이 callable을 감싸는 `Job`이 자동 생성된다.

---

## 7.4 job 자체 cancel 관련 단순화

현 시점에서는 **주석 수준에서 “job 자체는 cancel을 지원하지 않는다”**고 단순화해 둘 수 있다.

즉:

* 외부에서 개별 job cancel API를 적극 노출하지 않는다.
* 내부 상태로 `Canceled`는 가질 수 있으나,
* 외부 계약은 일단 cancel을 핵심 기능으로 보지 않는다.

이 부분은 이후 필요성이 커질 경우 다시 확장 가능하다.

> 주석 메모: 현재 단계에서는 “job은 cancel 안 됨”으로 단순화한다.

---

## 8. 멤버 함수 바인딩 지원

외부 API는 일반 lambda / functor뿐 아니라, 특정 클래스의 멤버 함수 등록을 지원해야 한다.

예상 형태:

```cpp id="hbg7h9"
template<typename T, typename Ret, typename... Args>
Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args);
```

이를 통해 다음이 가능해야 한다.

* 특정 owner 객체의 멤버 함수를 job으로 등록
* 실행 시점까지 owner 생존 보장 가능
* 일반 lambda와 동일한 사용성 유지

---

## 8.1 바인딩 정책 논의 메모

이 부분은 아직 확정이 아니라 논의 항목으로 남긴다.

### strong bind

* `shared_ptr` owner를 잡고 실행
* 실행 보장 우선
* owner lifetime 연장 가능

### weak bind

* `weak_ptr` owner를 잡고 실행 시 lock
* owner가 없으면 skip
* 객체 생명주기를 억지로 늘리지 않음

> 주석 메모: strong bind / weak bind 중 무엇을 기본 정책으로 할지 추후 논의 필요

---

## 9. 예약 실행

job은 즉시 실행뿐 아니라 예약 실행도 가능해야 한다.

즉, 개별 job 단위로 다음 실행 방식이 가능해야 한다.

* `PushAt`
* `PushAfter`

이는 `JobQueue` 경유 job에도 적용 가능하고,
필요하다면 `ThreadPool` 직접 실행 경로에도 적용 가능해야 한다.

즉:

* job은 단순 즉시 실행 객체가 아니라
* 예약 시점 정보까지 포함 가능한 실행 단위여야 한다.

---

## 10. Repeat 정책

## 10.1 지원 모드

`Repeat`는 다음 두 모드를 모두 지원해야 한다.

* `FixedRate`
* `FixedDelay`

---

## 10.2 repeat 재등록 방식

repeat는 실행 종료 후 다시 scheduler에 등록하는 방식으로 처리한다.

즉:

* 각 회차는 개별 job instance처럼 동작
* 실행 종료 후 다음 시각 계산
* `TimedJobManager`에 재등록

---

## 10.3 executeNow

`executeNow == true`이면 첫 실행은 즉시 시작 가능해야 하며, 이후부터 반복 스케줄 정책을 따른다.

---

## 10.4 shutdown 시 repeat 처리

shutdown 이후에는:

* future timed job 승격 금지
* repeat의 미래 회차 생성 금지

를 원칙으로 한다.

---

## 11. JobQueue 상태

`JobQueue`는 최소한 다음 상태를 가져야 한다.

* `Running`
* `Draining`
* `Stopped`

추가로 내부 관측용 보조 상태를 둘 수 있다.

예:

* idle
* empty
* active

이는 외부 API 상태와는 분리된 내부 상태로 취급한다.

---

## 12. ready queue draining 모델

다음 요구사항은 확정이다.

* 같은 `JobQueue`에 대해 동시에 두 drainer 금지
* 실행 누락 금지
* 너무 긴 독점 실행 방지

---

## 12.1 drain ownership

각 `JobQueue`는 drainer 존재 여부를 나타내는 atomic 상태를 가져야 한다.

이 상태는 단순히 task 등록 여부가 아니라,
**현재 이 queue를 책임지는 drainer가 있는가**를 나타내야 한다.

---

## 12.2 push 시 동작

ready queue에 새 job을 넣은 뒤:

* drainer가 없으면 drain task를 등록
* drainer가 있으면 추가 등록하지 않음

구현은 `exchange` 기반으로 단순화 가능하다.

---

## 12.3 drain 시 동작

drain task는 다음과 같이 동작해야 한다.

* ready queue에서 job을 꺼내 순차 실행
* 한 번에 처리할 최대 개수는 `BatchLimit`
* `BatchLimit` 도달 시 재등록 후 반환 가능
* queue가 비었다고 판단한 경우에만 ownership 해제
* 해제 직후 race 고려 후 필요 시 다시 drain 시작

---

## 12.4 batch limit

초기 `BatchLimit` 값은 **256**으로 한다.

또한 다음 항목을 계측할 수 있어야 한다.

* batch limit 도달 횟수
* drain 재등록 횟수
* drain당 처리량
* 연속 재등록 빈도

이를 통해 특정 queue의 과점유나 fairness 문제를 관측할 수 있어야 한다.

---

## 13. 종료 동작

## 13.1 신규 등록 차단

종료 절차 시작 후 신규 등록은 거부되어야 한다.

---

## 13.2 stop 옵션

종료 정책은 `Stop(args)` 형태로 선택 가능해야 한다.

예:

* reject new and drain
* reject new and discard pending
* timeout 기반 drain

---

## 13.3 상태 전이

최소 상태 전이는 다음과 같다.

* `Running`
* `Draining`
* `Stopped`

---

## 13.4 timed / repeat shutdown 처리

shutdown 진입 이후에는:

* future timed job 승격 중단
* repeat 미래 회차 생성 중단
* timer 엔트리 폐기 또는 무효화 가능

---

## 14. 통계 및 관측성

## 14.1 목적

운영 중 다음을 파악할 수 있어야 한다.

* 병목
* 정체
* 실패 증가
* 특정 thread 집중 현상
* 장시간 실행 job
* queue 대기 증가
* drain fairness 문제

---

## 14.2 통계 분류

### 누적 통계

* 총 등록 수
* 총 실행 수
* 총 완료 수
* 총 실패 수
* 총 거부 수

### 초당 통계

* 초당 실행 수
* 초당 완료 수
* 초당 실패 수
* 초당 유입량 / 처리량

### 분포 통계

* wait time histogram
* execution time histogram
* p50 / p95 / p99 wait
* p50 / p95 / p99 execution

---

## 14.3 히스토그램 방식

히스토그램은 **고정 배열 버킷 기반**으로 수집한다.

원칙은 다음과 같다.

* 외부 노출 값은 근사치임을 명시
* 짧은 구간은 촘촘하게
* 긴 구간은 넓게
* 비선형 버킷 구성이 바람직

즉, 정확한 raw value 전부를 보관하기보다 운영 관측용 근사 분포를 우선한다.

---

## 14.4 추가 계측 항목

가능하다면 다음도 계측할 수 있으면 좋다.

* I/O 대기 시간
* lock 경합 시간
* queue lock 보유 시간
* scheduler 처리 비용
* timer 승격 비용

---

## 14.5 slow job threshold

slow job threshold를 설정 가능해야 한다.

* threshold 초과 시 warning log 남김
* queue별 / 전역 설정 가능성이 있으면 좋음

---

## 15. 테스트 및 디버깅 지원

## 15.1 clock 주입

`TimedJobManager`는 clock 주입이 가능해야 한다.

* 운영: real clock
* 테스트: fake clock

---

## 15.2 executor 주입

실행기 역시 주입 가능해야 한다.

* 운영: `ThreadPool`
* 테스트: manual executor / inline executor

---

## 15.3 수동 실행 지원

테스트 / 디버깅 환경에서 다음이 가능하면 좋다.

* 시간 전진
* due job 승격
* queue drain 수동 호출
* 특정 횟수 pump

---

## 16. API 방향성 초안

구체 시그니처는 추후 설계에서 확정하되, 최소한 다음 범주의 API가 필요하다.

### 실행 등록

* `Post(callable)`
* `Push(callable)`
* `PushAt(time, callable)`
* `PushAfter(duration, callable)`
* `Repeat(period, callable, executeNow, mode)`

### 상태

* `GetState()`
* `GetPendingCount()`
* `GetTimedWaitingCount()`

### 종료

* `Stop(options)`

### 통계

* `GetStatsSnapshot()`
* `GetHistogram()`
* `GetRateStats()`

### 테스트 / 디버깅

* `Pump()`
* `AdvanceClockTo()`
* `FlushDueJobs()`

---

## 17. 논의 메모

다음 항목은 아직 확정하지 않고 주석/논의 항목으로 남긴다.

1. job 외부 cancel 지원을 다시 열지 여부
2. strong bind / weak bind 중 기본 정책
3. `ThreadPool` 직접 예약 실행 API 범위
4. `Post`의 inline 실행 허용 범위
5. bounded queue의 blocking 정책 상세
6. 히스토그램 버킷 경계값 설계
7. I/O / lock contention 계측 현실 범위

---

## 18. 구조 요약

본 요구사항은 다음 구조를 전제로 한다.

* `ThreadPool`

   * low-level executor
   * 단일 job 직접 실행 가능
* `JobQueue`

   * strand-like serialized executor
   * 같은 queue 동시 실행 금지
   * drain ownership 및 queue 실행 보장 담당
* `TimedJobManager`

   * 예약 / 반복 실행 관리
* `Job`

   * 내부 실행 단위 객체
   * 상태와 function 및 메타데이터 보유

---

원하시면 다음에는 이걸 바로 이어서
**“문서용 C++ 인터페이스 초안”** 형식으로 정리해드릴게요.
