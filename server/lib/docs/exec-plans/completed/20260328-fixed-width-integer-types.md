# Exec Plan: 고정 폭 정수 타입 헤더 추가

- **날짜**: 2026-03-28
- **상태**: completed
- **담당 영역**: `Lib/include/types/`
- **노출 방식**: 전역 타입 별칭

---

## 목표

게임 코드에서 자주 쓰는 고정 폭 정수 별칭을 공용 헤더로 제공한다.
서버와 향후 언리얼 클라이언트가 동일한 이름으로 사용할 수 있게 `UInt8`, `UInt16`, `UInt32` 같은 별칭을 한곳에 모은다.

---

## 범위

### 1. 공용 타입 헤더 추가

- `Lib/include/types/type_def.h`를 추가한다.
- `<cstdint>` 기반으로 고정 폭 정수 별칭을 정의한다.
- 우선 포함 대상은 아래 타입으로 제한한다.
  - `Int8`, `Int16`, `Int32`, `Int64`
  - `UInt8`, `UInt16`, `UInt32`, `UInt64`

### 2. 사용 위치 정리

- 전역에서 바로 사용할 수 있게 타입 별칭을 노출한다.
- 헤더 온리로 유지하고 외부 의존성은 추가하지 않는다.
- 기존 `Lib/include/types/` 구조와 충돌하지 않도록 파일명과 include 경로를 맞춘다.

### 3. 문서 반영

- 필요하면 `Lib/README.md` 또는 `Lib/include/README.md`에 타입 헤더 위치를 짧게 반영한다.

---

## 구현 방침

- 표준 라이브러리 `<cstdint>`만 사용한다.
- 파일명은 저장소 규칙에 맞춰 `snake_case`를 사용한다.
- 타입 이름은 게임 코드 관례에 맞춰 PascalCase 별칭을 사용한다.
- 플랫폼별 크기 차이가 있는 기본형(`int`, `long`)의 직접 별칭은 만들지 않는다.

---

## 진행 단계

- [x] 1. `Lib/include/types/type_def.h` 추가
- [x] 2. 필요 시 `Lib/include/README.md` 또는 `Lib/README.md` 갱신
- [x] 3. include 경로와 네임스페이스가 현재 구조와 맞는지 확인
- [x] 4. 완료 시 active index 상태 갱신

---

## 완료 기준

- `Lib/include/types/type_def.h`가 추가됨
- 게임 코드에서 사용할 `Int8`/`UInt8` 계열 별칭이 제공됨
- 외부 의존성 없이 공용 헤더로 재사용 가능함
