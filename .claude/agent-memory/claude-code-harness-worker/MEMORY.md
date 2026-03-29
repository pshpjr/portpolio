# Worker Agent Memory

## Project Context

- 저장소는 모노레포이며 루트 `AGENTS.md`가 첫 진입점이다.
- 앱 구현 전에는 대상 앱의 `AGENTS.md`를 먼저 읽는다.
- 루트 작업은 `docs/exec-plans/active/`의 exec-plan을 기준으로 진행한다.

## Patterns

- 계획을 먼저 만들고 범위를 유지한 채 구현한다.
- 기존 사용자 변경을 되돌리지 않는다.
- 문서/skill/agent는 저장소 루트의 공용 위치를 우선한다.
