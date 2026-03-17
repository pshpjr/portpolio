# Active Exec Plans
> 활성 실행 계획의 얇은 인덱스.
> 새 세션에서는 이 파일만 먼저 읽고, 관련 있는 plan 본문만 연다.

## 현재 활성 계획

- [20260317-add-utility-classes.md](./20260317-add-utility-classes.md) - NotNull\<T\> 등 경량 유틸리티 클래스 추가

### `20260314-project-setup.md`

- 목적: 서버 프로젝트 초기 설정과 빌드/검증 기반 정리
- 주 영향 경로: `CMakeLists.txt`, `CMakePresets.json`, `tools/`, `tests/`, 문서 전반
- 이 plan을 읽어야 하는 경우:
  - 빌드 체인, preset, vcpkg, 기본 검증 루틴을 수정할 때
  - 저장소 초기 scaffold나 공통 개발 환경을 건드릴 때

### `20260315-monorepo-server-move.md`

- 목적: 서버 폴더 기준 모노레포 재배치 후 구조 정리
- 주 영향 경로: `server/` 루트 구조, 문서 링크, 경로 기준
- 이 plan을 읽어야 하는 경우:
  - 경로 이동, 문서 링크 갱신, 루트/앱 진입점 구조를 수정할 때
  - 모노레포 기준선 자체를 바꿀 때

- [20260316-exec-plan-gate-and-skill.md](./20260316-exec-plan-gate-and-skill.md) - 작업 전 exec-plan 강제 규칙과 skill 추가
- [20260316-session-class-architecture.md](./20260316-session-class-architecture.md) - 세션 클래스 책임, 상태, 인터페이스 경계를 구현 전에 합의

## 운영 규칙

- 작업과 직접 무관한 활성 plan은 읽지 않는다.
- 작업 완료 후 관련 plan의 진행 상태만 갱신한다.
- 완료된 plan은 `completed/`로 이동한다.
