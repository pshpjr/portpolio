# Exec Plan: Harness Setup Bootstrap

## 목표

- 이 저장소에서 이미 사용 중인 하네스 흐름을 루트 기준으로 다시 작동 가능한 상태로 정리한다.
- 문서 전용 구성에 머물러 있는 현재 `.codex/`와 `.claude/`를 하네스 기본 골격까지 확장한다.
- 루트 작업자가 다음 세션부터 계획, 에이전트, 메모리 경로를 일관되게 사용할 수 있게 한다.

## 범위

- 루트 진입 파일:
  - `CLAUDE.md`
  - `Plans.md`
- Claude 설정:
  - `.claude/settings.json`
  - `.claude/hooks.json`
  - `.claude/agent-memory/claude-code-harness-worker/MEMORY.md`
  - `.claude/agent-memory/claude-code-harness-reviewer/MEMORY.md`
  - 필요 시 `.claude/agent-memory/claude-code-harness-scaffolder/MEMORY.md`
- 프로젝트 agent 프롬프트:
  - `.codex/agents/worker.md`
  - `.codex/agents/reviewer.md`
  - `.codex/agents/scaffolder.md`
  - `.claude/agents/worker.md`
  - `.claude/agents/reviewer.md`
  - `.claude/agents/scaffolder.md`
- 상위 문서 연계:
  - 필요 시 `docs/workflow/agent-workflow.md`
  - 필요 시 루트 `AGENTS.md`

## 완료 기준

- [x] 루트 `CLAUDE.md`와 `Plans.md`가 저장소 구조에 맞는 최소 하네스 진입점으로 준비된다.
- [x] `.claude/settings.json`과 `.claude/hooks.json`이 현재 저장소에서 안전하게 사용할 수 있는 기본값으로 추가된다.
- [x] worker/reviewer/scaffolder용 agent 프롬프트가 `.codex/agents/`와 `.claude/agents/`에 존재한다.
- [x] `.claude/agent-memory/` 아래에 하네스 에이전트 메모리 템플릿이 준비된다.
- [x] 기존 문서 전용 agent/skill 구성과 충돌하지 않는다는 점을 검토한다.
- [x] 변경 후 최소 파일 존재 검증과 문서/설정 정합성 확인을 수행한다.

## 진행 상황

- [x] 루트 `AGENTS.md`, `docs/AGENTS.md`, 활성 plan 인덱스 확인
- [x] 기존 `.codex/`, `.claude/` 자산과 누락 범위 조사
- [x] 하네스 셋업용 exec-plan 초안 작성
- [x] `exec-plan-gate`를 plan 선행용 흐름으로 조정
- [x] 실제 하네스 파일 생성
- [x] 생성 결과 검증 및 plan 상태 갱신

## 설계 결정 기록

- 이번 작업은 특정 앱 구현이 아니라 저장소 전역 작업자 환경을 정비하는 셋업이므로 루트 `docs/exec-plans/active/`에서 관리한다.
- 이미 존재하는 문서 전용 skill/agent 자산은 유지하고, 하네스 공용 골격만 추가하는 방향을 기본으로 한다.
- 하네스 기본 골격은 루트 작업 흐름에 필요한 최소 파일부터 복원하고, 앱별 세부 설정은 각 앱의 `AGENTS.md` 체계에 맡긴다.
- 사용자 요청이 "이미 쓰고 있었다"는 전제를 포함하므로, 기존 자산을 대체하기보다 누락된 공용 진입점과 표준 파일을 보강한다.
- `exec-plan-gate`는 사용자 승인 대기를 강제하지 않고, plan 선행과 범위 유지에 초점을 두도록 조정한다.
- `.claude/settings.json`과 `.claude/hooks.json`은 현재 저장소에 맞는 최소 안전 기본값부터 두고, 실제 실행 훅은 이후 필요 시 확장한다.
