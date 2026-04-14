# Exec Plan: Boost MySQL User DB Executor

## Goal

`boost-mysql`을 서버 종속성으로 추가하고, 유저 단위로 직렬화된 DB 처리 경로를 만든다.

실행 체인:

```
Session(Network)
  → Field strand (게임 로직, 호출자가 소유)
    → User DB strand (유저 단위 DB 직렬화, DbExecutor io_context 소속)
      → DB connection pool (boost::mysql, 코루틴)
    → onComplete(ec)  // User DB strand 위에서 호출.
                      // 필요 시 호출자가 캡처한 field executor로 재-post.
```

- `DbExecutor`는 자체 `io_context`와 워커 스레드를 소유한다. 외부에서 io_context를 주입하지 않는다.
- User의 DB strand는 `DbExecutor`의 executor에서 파생된다.
- `IQuery` 인터페이스가 boost::mysql 바인드된 쿼리를 캡슐화한다.
- `DbExecutor::Post`는 완료 executor를 받지 않는다. 콜백은 User DB strand 위에서 호출되며, 호출자가 필드 strand 등 최종 타깃을 콜백 람다 안에 직접 캡처해 re-dispatch 한다.

---

## Scope

### 빌드/종속성

- `server/vcpkg.json`에 `boost-mysql` 추가 (`1.87.0` 기준, 나머지 Boost 버전과 정합).
- `server/CMakeLists.txt`에서 `find_package(Boost ... COMPONENTS system)`에 필요한 추가 컴포넌트가 있으면 확장 (boost-mysql은 헤더 온리 + OpenSSL 런타임).
- OpenSSL(`find_package(OpenSSL)`)도 링크. boost-mysql이 SSL 필요.
- `portpolio_server`의 PUBLIC/PRIVATE 링크 구성 갱신.

### 디렉터리 배치

현재 `src/` 는 평탄 구조 (`common`, `network`, `user`, ...). DB IO는 Network와 동급 IO 계층이므로 `src/database/` 아래에 둔다.

- `src/database/i_query.h` — IQuery 인터페이스
- `src/database/db_executor.h|.cpp` — DbExecutor 구현
- `src/database/db_callback.h` — 콜백 타입 alias (필요 시 i_query.h에 흡수)

### IQuery 인터페이스

```cpp
class IQuery
{
public:
    virtual ~IQuery() = default;

    // 코루틴 내부에서 호출. boost::mysql::any_connection 을 받아
    // 바인드된 statement/쿼리를 실행하고 결과를 내부 상태로 저장.
    virtual boost::asio::awaitable<std::error_code>
        Execute(boost::mysql::any_connection& conn) = 0;
};
```

- 파생 쿼리는 내부에 prepared statement, 바인드 파라미터, 결과 저장 공간을 포함한다.
- boost::mysql의 `results`, `static_results<Row>` 등을 파생 쿼리가 소유한다.
- 예외 금지 컨벤션에 따라 코루틴 내부에서는 `as_tuple(deferred)` 또는 `redirect_error`로 에러 코드를 반환.

### DbExecutor

- 자체 `boost::asio::io_context`와 워커 스레드(`std::jthread` 1개 이상)를 소유.
- 생성자: `boost::mysql::pool_params`와 워커 수 정도만 받는다.
- 내부에 `boost::mysql::connection_pool` 소유, `Start()`에서 `async_run` 시작, `Stop()`에서 `pool.cancel()` → `io_context.stop()` → 스레드 join.

유저 DB strand 위에서 `co_spawn` 으로 쿼리를 실행하고, 완료 시 같은 strand 위에서 `onComplete`를 호출한다. 필드 strand 등 최종 목적지는 호출자가 콜백 람다 내부에 캡처로 넣어 직접 dispatch 한다.

외부 API:

```cpp
using DbStrand = boost::asio::strand<boost::asio::io_context::executor_type>;
using DbCallback = std::function<void(std::error_code)>;

class DbExecutor
{
public:
    DbExecutor(boost::mysql::pool_params params,
               std::size_t              workerCount = 1);
    ~DbExecutor();

    DbExecutor(const DbExecutor&) = delete;
    DbExecutor& operator=(const DbExecutor&) = delete;

    void Start();
    void Stop();

    // User DB strand 를 생성해 반환. User 는 이 strand 를 소유.
    DbStrand MakeDbStrand();

    // userDbStrand : DB 접근을 유저 단위로 직렬화하는 strand (User 소유).
    // query        : bound parameters / 결과 저장소를 내부에 소유.
    // onComplete   : userDbStrand 위에서 호출됨.
    //                호출자가 field strand 등 최종 타깃을 캡처로 넣어 재-dispatch 한다.
    void Post(const DbStrand&         userDbStrand,
              std::shared_ptr<IQuery> query,
              DbCallback              onComplete);
};
```

`Post` 내부 코루틴 흐름:

1. `co_spawn(userDbStrand, RunOnStrand(...), detached)`.
2. `RunOnStrand`:
   - `pool.async_get_connection(as_tuple(use_awaitable))`로 connection 획득.
   - `co_await query->Execute(conn)` → `std::error_code`.
   - connection 파괴 시 풀로 반환.
   - `onComplete(ec)` 를 strand 위에서 직접 호출 (현재 실행 컨텍스트 = userDbStrand).
- 예외 금지 정책: 코루틴 내부는 `as_tuple(use_awaitable)` 기본 사용. `co_spawn`의 exception handler는 즉시 종료(`std::abort`).

### User 클래스

- `src/user/user.h`에 DB strand 추가.
  - 타입: `boost::asio::strand<boost::asio::io_context::executor_type>`
  - 생성자에서 `DbExecutor&` 를 받아 `dbExecutor.MakeUserStrand()` 로 초기화.
- 외부 호출부(Field 등)는 아래처럼 사용:

```cpp
dbExec.Post(
    user.DbStrand(),
    std::move(query),
    [fieldStrand = field.Strand(), /* other captures */](std::error_code ec) mutable
    {
        boost::asio::post(fieldStrand, [ec]{ /* 필드 strand 복귀 후 처리 */ });
    });
```

- `const DbStrand& DbStrand() const` 게터 제공.
- 기존 `uid_` 멤버 네이밍을 컨벤션(`m_uid`)에 맞춰 정리.
- Field 타입은 이번 plan 범위 밖. 이번 plan은 User strand 와 DbExecutor 계약까지만 확정한다.

### 범위 외

- 실제 쿼리 구현(로그인, 인벤토리 등).
- connection_pool 튜닝, 리트라이 정책, 트랜잭션 API.
- Runtime 엔트리포인트 연결 및 설정 로딩(별도 plan).
- 테스트: 단위 테스트는 boost-mysql이 실제 DB를 요구하므로 이번 plan에서는 smoke build만 보장. 통합 테스트는 별도 plan.
- 예외 정책 완화, 로깅 백엔드 변경.

---

## Completion Criteria

- `server/vcpkg.json`에 `boost-mysql` 추가, cmake configure 통과.
- `DbExecutor`가 헤더·구현 형태로 존재하고 `portpolio_server` 타깃에 포함.
- `IQuery` 인터페이스가 정의되어 있고 `DbExecutor::Post`의 파라미터로 수용.
- `User`가 DB strand를 소유하고 외부에 노출.
- `portpolio_server` 빌드 통과.
- `tools/check_layers.py` 통과 (database 레이어 규칙은 network와 동급 IO로 취급, core 역참조 없음).
- exec-plan 상태 갱신 및 완료 시 `completed/`로 이동.

---

## Progress

- [completed] exec-plan 사용자 검토 및 승인
- [completed] vcpkg.json에 boost-mysql 추가, CMake 구성 업데이트
- [completed] `src/database/i_query.h` 추가
- [completed] `src/database/db_executor.h|.cpp` 추가
- [completed] `src/user/user.h|.cpp`에 DB strand 추가 (DbExecutor::MakeDbStrand 통해 초기화) 및 네이밍 정리
- [pending] 빌드 검증 (로컬 셸에 cmake 미등록, 사용자 측 CLion/Dev Shell 에서 확인 필요)
- [pending] 빌드 통과 후 plan 완료 처리 및 `completed/` 이동

---

## Design Decision Log

- **boost::mysql 코루틴 채택 이유**: 사용자가 요구. 풀 + strand + 코루틴은 boost::mysql 공식 권장 구조와 일치하고, 콜백 체이닝보다 상태 추적이 단순.
- **IQuery를 추상 인터페이스로 두는 이유**: 쿼리별 파라미터·결과 타입이 다르기 때문에 `DbExecutor`는 실행 계약만 알면 된다. 파생 쿼리가 상태를 캡슐화하고 호출자가 소유권을 관리.
- **DbExecutor가 자체 io_context를 소유**: DB IO는 게임 루프 io_context와 스레드를 공유하지 않는다. 쿼리 지연이 게임 루프를 블로킹하지 않도록 분리, 운영 중 DB 풀을 독립적으로 stop/start 가능.
- **strand는 User 소유, DbExecutor가 팩토리 제공**: User DB strand는 DbExecutor io_context에서 파생되어야 유효하므로 `DbExecutor::MakeUserStrand()` 를 통해 생성. 소유권은 User 가 갖는다.
- **완료 콜백은 User DB strand 위에서 호출**: DbExecutor는 완료 executor 파라미터를 받지 않는다. 필드 strand 등 최종 목적지는 호출자가 콜백 람다에 직접 캡처해 `boost::asio::post(fieldStrand, ...)` 로 재-dispatch 한다. 이 규약은 DbExecutor 인터페이스를 단순하게 유지하고, 완료 대상의 선택권을 호출자에게 온전히 위임한다.
- **예외 금지 정책 대응**: boost::mysql API는 기본적으로 예외를 던질 수 있으므로 `as_tuple(use_awaitable)` 완료 토큰을 기본 사용해 `std::error_code`로 라우팅한다. 코루틴 경계의 fall-through 예외는 `co_spawn(..., [](std::exception_ptr){ std::abort(); })` 형태로 즉시 종료.
- **디렉터리 배치**: 기존 `src/` 가 평탄 구조이므로 새 IO 모듈은 `src/database/` 로 일관 배치. 추후 레이어 번호 마이그레이션 때 `05_network` 와 함께 `05_io_*` 계열로 정돈 예정.
