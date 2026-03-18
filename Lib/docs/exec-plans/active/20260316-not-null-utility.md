# NotNull 유틸리티 클래스 구현

## 목표

`shared_ptr`의 null 여부를 컴파일 타임 또는 생성 시점에 한 번만 검사하고, 이후 사용처에서 null 체크를 완전히 제거하는 `NotNull<T>` 타입 안전 래퍼를 `Lib/`에 추가한다.

## 범위

- `Lib/include/types/not_null.h` — NotNull 클래스 헤더 (헤더 온리)
- `Lib/tests/types/not_null_test.cpp` — 단위 테스트
- `Lib/CMakeLists.txt` — Lib 전용 빌드/테스트 구성
- `Lib/vcpkg.json` — Lib 테스트 의존성 manifest
- `Lib/README.md` — 구조 갱신
- `Lib/docs/exec-plans/active/INDEX.md` — 이 plan 등록
- `Lib/include/types/` — 공용 타입 안전 래퍼 헤더 디렉터리
- `Lib/tests/types/` — 타입 계열 단위 테스트 디렉터리

## 완료 기준

- [x] `NotNull<T>::Make(Args&&...)` — 내부에서 `make_shared` 후 NotNull 반환
- [x] `NotNull<T>::From(shared_ptr<T>&&)` — null이면 `nullopt`, 아니면 `NotNull` 반환
- [x] 복사/이동 생성자·대입 연산자 제공
- [x] `operator->()`, `operator*()` 역참조 제공
- [x] `operator shared_ptr<T>()` 암시적 변환 제공
- [x] `to_weak()` → `weak_ptr<T>` 변환 메서드 제공
- [x] 네임스페이스: `portpolio::lib::types`
- [x] `Lib/CMakeLists.txt`로 헤더 포함 경로와 테스트 타깃 구성
- [x] `Lib/vcpkg.json`으로 테스트 의존성 해석 가능
- [x] 단위 테스트 작성 및 통과

## 설계 결정 기록

- 내부 멤버는 `shared_ptr<T>` 하나만 보유한다.
- `From()`은 `shared_ptr<T>&&` rvalue reference만 받는다. 호출자는 반드시 `std::move()`를 명시해야 하며, 소유권 이전 의도를 컴파일 타임에 강제한다.
- `NotNull` 객체가 존재하는 한 내부 포인터는 항상 non-null임을 불변식으로 보장한다.
- 기본 생성자는 제공하지 않는다 (null 상태 진입 차단).
- `shared_ptr`과의 암시적 변환은 `operator shared_ptr<T>()`로 제공한다.
- 헤더 온리로 구현하여 Lib 빌드 시스템 의존성 없이 서버/클라이언트 양쪽에서 include만으로 사용 가능하게 한다.
- `Lib/`는 독립 `CMakeLists.txt`를 가져야 하며, NotNull 테스트는 우선 이 경로에서 직접 빌드/실행 가능해야 한다.
- `Lib` 단독 테스트 configure는 `Lib/vcpkg.json` manifest를 통해 `GTest`를 해석한다.

## 진행 상황

- [x] exec-plan 작성 및 사용자 검토
- [x] exec-plan 범위/시그니처 정리 반영
- [x] `not_null.h` 구현
- [x] `not_null_test.cpp` 작성
- [x] `Lib/CMakeLists.txt` 작성
- [x] `Lib/vcpkg.json` 작성
- [x] 빌드/테스트 검증

## 검증 메모

- 최종 검증 경로는 `Lib/CMakeLists.txt`를 통한 독립 빌드/테스트 구성이다.
- 로컬 `cl` 프로브 컴파일 및 실행으로 `not_null.h`의 기본 생성/변환 경로는 확인했다.
- 전체 `cmake` configure/build/test는 vcpkg가 Git/패키지 다운로드를 시도하다 네트워크 오류(`WinHttpSendRequest failed with exit code 12029`)로 중단되었다.
- 다음 단계에서는 `Lib/CMakeLists.txt`를 추가하고, 권한 상승 후 네트워크 가능한 환경에서 Lib 단위 테스트를 재검증한다.
- 최종 검증은 `Lib/CMakeLists.txt` + `Lib/vcpkg.json` 조합으로 `x64-windows-static` triplet에서 수행했고, `ctest` 기준 7개 테스트가 모두 통과했다.
