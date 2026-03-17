# Exec Plan: Add Utility Classes

## Goal

세션 클래스 구현 전에 공통으로 필요한 경량 유틸리티 클래스를 `src/01_types/` 에 추가한다.

---

## Scope

### `NotNull<T>`

- non-null 포인터 계약을 타입 수준에서 강제하는 래퍼 템플릿
- 생성자에서 null 여부를 `assert`로 검사
- `operator->()` / `operator*()` 는 const 오버로드 쌍으로 제공
  - `NotNull<T>&` → `T*` / `T&` 반환 (비-const 호출 허용)
  - `const NotNull<T>&` → `const T*` / `const T&` 반환 (const 메서드만 허용)
  - **deep const 전파는 적용하지 않는다** (의도적으로 제외)
- `get()` 접근자 제공 (`T*` 반환)
- 복사/이동 허용, null 재할당은 금지하지 않음 (T가 포인터/스마트포인터일 때 자연스럽게 동작)
- `shared_ptr<T>` / `T*` 모두 수용할 수 있도록 템플릿 파라미터를 제한하지 않음

### 범위 외

- Ring buffer, send queue 등 Network 레이어 전용 버퍼 구조 → 별도 plan
- deep const propagation (`propagate_const` 패턴) → 사용하지 않기로 합의

---

## Placement

| 파일 | 위치 |
|------|------|
| `not_null.h` | `server/src/01_types/not_null.h` |

- `src/01_types/` 는 외부 의존성이 없어야 하므로 STL (`<cassert>`, `<utility>`) 이외 헤더 포함 금지

---

## Interface Sketch

```cpp
// server/src/01_types/not_null.h

template <typename T>
class NotNull {
public:
    explicit NotNull(T ptr) noexcept;        // assert(ptr != nullptr)

    T        operator->()       noexcept;
    const T  operator->() const noexcept;    // T가 포인터/스마트포인터이면 const 메서드만 호출 가능

    auto&        operator*()       noexcept;
    const auto&  operator*() const noexcept;

    T        get()       noexcept;
    const T  get() const noexcept;

    // null 재할당 금지 편의용 — 필요 시 추가
    NotNull& operator=(std::nullptr_t) = delete;
};
```

> `const T operator->() const` 에서 T 가 `Foo*` 이면 `const Foo*` 를 돌려주는 효과.
> T 가 `shared_ptr<Foo>` 이면 반환 타입이 `const shared_ptr<Foo>` 가 되어 `->` 재호출 시 `Foo const*` 로 전파된다.

---

## Completion Criteria

- `not_null.h` 헤더가 `src/01_types/` 에 추가된다.
- `T = Foo*` 와 `T = std::shared_ptr<Foo>` 양쪽에서 const 오버로드가 의도대로 동작하는 단위 테스트가 통과한다.
- `check_layers.py` 레이어 검사를 통과한다.
- CI 빌드가 통과한다.

---

## Progress

- [pending] exec-plan 사용자 검토 및 승인
- [pending] `src/01_types/not_null.h` 헤더 작성
- [pending] `tests/` 에 `not_null_test.cpp` 단위 테스트 추가
- [pending] 레이어 검사 및 빌드 검증
- [pending] 커밋 & 푸시, plan completed/ 이동
