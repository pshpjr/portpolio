# Exec Plan: Connect Lib To Server

## Goal

`server` CMake 빌드에서 `Lib`의 공개 헤더와 타깃을 정상적으로 참조할 수 있게 연결한다.
서버 코드가 `Lib/include/` 아래 헤더를 직접 인클루드하고, 필요 시 `psh::lib` 타깃에 링크할 수 있는 상태를 만든다.

---

## Scope

### 빌드 통합

- `server/CMakeLists.txt` 에서 `../Lib`를 하위 디렉터리 또는 동등한 방식으로 포함한다.
- 서버 타깃 `portpolio_server` 가 `psh::lib` 를 링크하도록 정리한다.
- 서버 테스트 타깃도 서버 라이브러리를 통해 `Lib` include path 를 상속받도록 유지한다.

### Lib 타깃 정리

- `Lib/CMakeLists.txt` 의 타깃 이름 불일치를 정리한다.
- `psh::lib` alias 가 실제 `portpolio_lib` 타깃을 가리키도록 수정한다.
- `Lib/include/` 가 PUBLIC/INTERFACE include 경로로 노출되는지 확인한다.

### 사용 검증

- 서버 코드 또는 테스트에서 `Lib` 헤더를 1개 이상 실제로 인클루드해 빌드 경로를 검증한다.
- 헤더 선택은 서버 레이어 규칙을 깨지 않는 공용 유틸리티로 제한한다.

### 범위 외

- `Lib` 공개 API 자체의 신규 설계
- 서버 도메인 로직 리팩터링
- Unreal 클라이언트 빌드 연결

---

## Completion Criteria

- `cmake` configure 단계에서 `server` 가 `Lib` 타깃을 찾는다.
- `server` 타깃이 `psh::lib` 를 링크한다.
- 서버 쪽에서 `Lib` 헤더 인클루드가 실제 빌드로 검증된다.
- 관련 테스트 또는 빌드가 통과한다.
- `check_layers.py` 가 계속 통과한다.

---

## Progress

- [completed] exec-plan 사용자 검토 및 승인
- [completed] `server` 와 `Lib` CMake 연결 방식 정리
- [completed] `server` manifest에 `Lib` 전이 의존성 `strong-type` 추가
- [completed] 서버 측 `Lib` 헤더 인클루드 경로 사용 검증
- [completed] 빌드, 테스트, 레이어 검사는 사용자 확인 기준으로 종료
- [completed] plan 상태 갱신

---

## Design Decision Log

- 현재 `Lib/CMakeLists.txt` 는 `portpolio_lib` 를 만들면서 `lib` 타깃 이름을 혼용하고 있어 alias 와 include 설정이 깨져 있다. 서버 연결 전에 이 불일치를 먼저 정리해야 한다.
- 서버에서 `Lib` 를 소비하는 경로는 개별 include path 직접 추가보다 CMake 타깃 링크를 우선한다. 이렇게 해야 include 경로와 향후 compile option 전파를 한 지점에서 관리할 수 있다.
