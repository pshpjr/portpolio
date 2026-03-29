# Assert 유틸리티 구현

## 목표

런타임 불변식 검증을 위한 3단계 Assert 유틸리티를 `Lib/`에 추가한다.
호출 위치(source_location)와 스택 트레이스를 포함한 진단 정보를 제공하며,
빌드 타입(Debug/Release)에 따라 동작이 달라진다.

## 범위

- `Lib/include/utils/assert.h` — Assert 유틸리티 헤더 (헤더 온리)
- `server/vcpkg.json` — `boost-stacktrace` 의존성 추가
- `server/CMakeLists.txt` — `Boost::stacktrace_basic` 링크 추가
- `Lib/tests/utils/assert_test.cpp` — 단위 테스트
- `Lib/AGENTS.md` — 네임스페이스 규칙 추가
- `Lib/docs/exec-plans/active/INDEX.md` — 이 plan 등록

## 3단계 설계

### `ASSERT(cond, msg)` — 디버그 전용, 예외 throw

| 빌드 | 동작 |
|------|------|
| Debug (`PORTPOLIO_DEBUG` 정의됨) | 조건 실패 시 source_location + 스택 트레이스 포함 `std::logic_error` throw |
| Release | empty inline (no-op). 조건 표현식은 평가됨 |

### `FATAL(cond, msg)` — 항상 활성, 예외 throw

| 빌드 | 동작 |
|------|------|
| Debug / Release 모두 | 조건 실패 시 source_location + 스택 트레이스 포함 `std::logic_error` throw |

### `ENSURE(cond, msg)` — 디버그 전용, stderr 출력 후 계속 실행

| 빌드 | 동작 |
|------|------|
| Debug (`PORTPOLIO_DEBUG` 정의됨) | 조건 실패 시 **callsite당 1회만** stderr에 source_location + 스택 트레이스 출력 후 실행 계속 |
| Release | empty inline (no-op). 조건 표현식은 평가됨 |

## 인터페이스 설계

매크로를 사용하는 이유:
1. `std::source_location::current()`는 호출 지점에서 캡처해야 함
2. `ENSURE`의 callsite별 1회 제한은 매크로 내 `static bool`로만 구현 가능
3. Release no-op 시에도 조건 표현식은 평가됨 (side-effect 보존)

```cpp
// assert.h 사용 예시

// ASSERT: 디버그에서만 검증
ASSERT(index < size, "index out of range");

// FATAL: 항상 검증 (프로덕션 포함)
FATAL(config != nullptr, "config must be initialized before use");

// ENSURE: 디버그에서 1회만 경고, 계속 실행
ENSURE(cache_hit_rate > 0.5, "cache hit rate unexpectedly low");
```

## 완료 기준

- [ ] `ASSERT(cond, msg)` 매크로 — Debug: throw, Release: no-op
- [ ] `FATAL(cond, msg)` 매크로 — 항상 throw
- [ ] `ENSURE(cond, msg)` 매크로 — Debug: callsite당 1회 stderr, Release: no-op
- [ ] `std::source_location` 파일명·줄번호·함수명 포함
- [ ] `boost::stacktrace` 스택 트레이스 포함 (throw 전, stderr 출력 시)
- [ ] throw 타입: `std::logic_error` (메시지에 진단 정보 포함)
- [ ] 네임스페이스: `psh::lib::utils` (내부 detail 함수), 매크로는 전역
- [ ] 헤더 온리
- [ ] `server/vcpkg.json`에 `boost-stacktrace` 추가
- [ ] `server/CMakeLists.txt`에 `Boost::stacktrace_basic` 링크 추가
- [ ] 단위 테스트 작성 및 통과

## 설계 결정 기록

- **매크로 이름 충돌**: `ASSERT` / `FATAL` / `ENSURE`는 프로젝트 전용 매크로로 간주한다.
  기존 `assert.h`의 `assert()` 매크로와 이름이 다르므로 충돌 없음.
- **조건 표현식 평가**: Release no-op에서도 `(void)(cond)` 형태로 표현식을 평가하여
  side-effect가 있는 코드에서도 동작이 일관됨.
- **스택 트레이스 백엔드**: `boost::stacktrace_basic` 사용.
  추가 플랫폼 라이브러리 없이 동작하는 가장 portable한 백엔드.
- **`PORTPOLIO_DEBUG` 매크로**: `NDEBUG` 대신 프로젝트 전용 매크로를 사용하여
  외부 라이브러리의 assert 동작에 영향을 주지 않음.
  CMake Debug 빌드 시 자동 정의됨 (`target_compile_definitions`).
- **throw vs terminate**: `fatal`은 스택을 풀어 RAII 소멸자가 동작하도록 throw를 선택.
  추후 커스텀 exception 타입으로 교체 가능하도록 `detail::throw_logic_error()` 헬퍼로 위임.
- **헤더 온리**: Lib 빌드 시스템 의존성 없이 서버/클라이언트 양쪽에서 include만으로 사용.
  단, 소비자(server)는 `boost-stacktrace` 링크 필요.

## 진행 상황

- [ ] exec-plan 작성 및 사용자 검토
- [ ] `server/vcpkg.json` boost-stacktrace 추가
- [ ] `server/CMakeLists.txt` 링크 추가
- [ ] `Lib/AGENTS.md` 네임스페이스 규칙 추가
- [ ] `assert.h` 구현
- [ ] `assert_test.cpp` 작성
- [ ] 빌드/테스트 검증
