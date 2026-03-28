# Exec Plan: Expose Lib To Server Build

## Goal

`Lib` CMake 타깃을 정리해서 `server` 가 공용 헤더와 라이브러리 타깃을 안정적으로 소비할 수 있게 한다.

---

## Scope

### 타깃 정의 정합성

- `Lib/CMakeLists.txt` 의 실제 타깃 이름과 alias 이름을 일치시킨다.
- `target_include_directories` 와 `target_compile_features` 가 올바른 타깃에 적용되도록 수정한다.

### 서버 소비 기준 보장

- `Lib/include/` 가 `psh::lib` 를 통해 노출되도록 유지한다.
- 서버가 `add_subdirectory(../Lib ...)` 또는 동등한 방식으로 소비할 수 있는 구조를 보장한다.

### 범위 외

- 공용 헤더 신규 추가
- `Lib` 테스트 구조 개편
- 클라이언트 소비 경로 추가

---

## Completion Criteria

- `psh::lib` alias 가 유효하다.
- `Lib/include/` 경로가 `psh::lib` 소비자에게 전파된다.
- `server` 에서 `Lib` 헤더 인클루드가 실제 빌드로 검증된다.

---

## Progress

- [completed] exec-plan 사용자 검토 및 승인
- [completed] `Lib/CMakeLists.txt` 타깃 이름 정리
- [completed] 서버 소비 시나리오 기준으로 include 전파 확인
- [completed] 관련 빌드/테스트 검증은 사용자 확인 기준으로 종료
- [completed] plan 상태 갱신

---

## Design Decision Log

- `Lib` 는 현재 헤더 온리 성격이 강하므로 `INTERFACE` 타깃 유지가 적절하다. 서버는 개별 경로 하드코딩 대신 `psh::lib` 링크로 소비한다.
