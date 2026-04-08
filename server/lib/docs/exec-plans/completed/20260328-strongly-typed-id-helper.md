# Exec Plan: Strongly Typed Id Helper

## Goal

`int` 같은 원시 정수 타입을 그대로 콘텐츠 아이템 ID, 스킬 ID 등 여러 도메인 식별자에 재사용하지 않도록 `rollbear/strong_type` 기반의 `Lib` 공용 강타입 ID 헬퍼를 추가한다.

---

## Scope

### 외부 라이브러리 기반 공용 타입 헬퍼 추가

- `Lib/vcpkg.json` 에 `strong-type` 의존성을 추가한다.
- `Lib/include/types/` 아래에 `rollbear/strong_type` 를 감싼 강타입 식별자 래퍼 헤더를 추가한다.
- 기본 저장 타입은 정수 계열을 우선 지원한다.
- 서로 다른 태그 타입이면 암시적 대입이나 비교가 되지 않도록 막는다.

### 사용성 기준

- 값 생성이 명확해야 한다.
- 기본 값 접근은 `strong::value_of()` 또는 래퍼 헬퍼로 명시적이어야 한다.
- 비교 연산과 해시처럼 식별자 용도로 자주 필요한 최소 기능을 제공한다.

### 테스트

- 같은 태그끼리만 비교 가능한지 검증한다.
- 다른 태그끼리 혼용이 컴파일 단계에서 차단되는지 정적 검증한다.
- 기본 값 접근과 해시 사용이 가능한지 단위 테스트로 확인한다.

### 범위 외

- 서버 도메인별 구체 ID 타입 대량 추가
- 직렬화 프레임워크 연동
- 문자열 기반 식별자 지원

---

## Candidate API

```cpp
namespace psh::lib::types {

template <typename Tag, typename ValueType = Int32>
using StrongId = strong::type<
    ValueType,
    Tag,
    strong::default_constructible,
    strong::equality,
    strong::ordered>;

template <typename Tag, typename ValueType = Int32>
constexpr ValueType& ValueOf(StrongId<Tag, ValueType>& value) noexcept;

}  // namespace psh::lib::types
```

- 해시는 `std::hash` 특수화로 제공한다.

---

## Completion Criteria

- [completed] `psh::lib::types` 네임스페이스 아래 강타입 ID 헬퍼 추가
- [completed] 태그가 다르면 타입이 구분되어 혼용 불가
- [completed] 원시 값 변환은 명시적 인터페이스만 제공
- [completed] 비교와 해시 지원
- [completed] `Lib/tests/types/` 단위 테스트 추가
- [completed] 관련 README 갱신

---

## Progress

- [completed] exec-plan 작성 및 사용자 검토
- [completed] 기존 `types` 계층 구조에 맞는 헤더 위치 확정
- [completed] 외부 강타입 라이브러리 존재 여부 확인
- [completed] `strong-type` 의존성 및 CMake 연결 반영
- [completed] `rollbear/strong_type` 기반 강타입 ID 헬퍼 구현
- [completed] 단위 테스트 작성
- [completed] 기존 `NotNull` 네임스페이스 기준을 `psh`로 유지하도록 정리
- [completed] 빌드 검증은 사용자 확인 기준으로 종료
- [completed] plan 상태 갱신
