# NotNull 유틸리티 클래스 구현

## 목표

`shared_ptr`의 null 여부를 컴파일 타임 또는 생성 시점에 한 번만 검사하고, 이후 사용처에서 null 체크를 완전히 제거하는 `NotNull<T>` 타입 안전 래퍼를 `Lib/`에 추가한다.

## 범위

- `Lib/include/types/not_null.h` — NotNull 클래스 헤더 (헤더 온리)
- `Lib/tests/types/not_null_test.cpp` — 단위 테스트
- `Lib/AGENTS.md` — 네임스페이스 규칙 추가
- `Lib/README.md` — 구조 갱신
- `Lib/docs/exec-plans/active/INDEX.md` — 이 plan 등록

## 완료 기준

- [ ] `NotNull<T>::make(Args&&...)` — 내부에서 `make_shared` 후 NotNull 반환
- [ ] `NotNull<T>::from(shared_ptr<T>)` — null이면 `nullopt`, 아니면 `NotNull` 반환
- [ ] 복사/이동 생성자·대입 연산자 제공
- [ ] `operator->()`, `operator*()` 역참조 제공
- [ ] `operator shared_ptr<T>()` 암시적 변환 제공
- [ ] `to_weak()` → `weak_ptr<T>` 변환 메서드 제공
- [ ] 네임스페이스: `portpolio::lib::types`
- [ ] 단위 테스트 작성 및 통과

## 설계 결정 기록

- 내부 멤버는 `shared_ptr<T>` 하나만 보유한다.
- `from()`은 `shared_ptr<T>&&` rvalue reference만 받는다. 호출자는 반드시 `std::move()`를 명시해야 하며, 소유권 이전 의도를 컴파일 타임에 강제한다.
- `NotNull` 객체가 존재하는 한 내부 포인터는 항상 non-null임을 불변식으로 보장한다.
- 기본 생성자는 제공하지 않는다 (null 상태 진입 차단).
- `shared_ptr`과의 암시적 변환은 `operator shared_ptr<T>()`로 제공한다.
- 헤더 온리로 구현하여 Lib 빌드 시스템 의존성 없이 서버/클라이언트 양쪽에서 include만으로 사용 가능하게 한다.

## 진행 상황

- [ ] exec-plan 작성 및 사용자 검토
- [ ] `Lib/AGENTS.md` 네임스페이스 규칙 추가
- [ ] `not_null.h` 구현
- [ ] `not_null_test.cpp` 작성
- [ ] 빌드/테스트 검증
