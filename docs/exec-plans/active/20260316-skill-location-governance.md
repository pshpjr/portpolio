# Exec Plan: Skill Location Governance

## 목표

- 저장소에서 skill과 agent를 두는 올바른 위치를 문서화한다.
- 잘못 생성한 `server/skills` 기반 skill을 제거하고 `.codex`, `.claude`로 이동한다.

## 범위

- `docs/AGENTS.md`
- `docs/workflow/agent-workflow.md`
- 필요 시 루트 `AGENTS.md`
- `.codex/skills/exec-plan-gate/`
- `.claude/skills/exec-plan-gate/`
- 잘못 생성한 `server/skills/exec-plan-gate/`

## 완료 기준

- 문서가 skill/agent 저장 위치를 `.codex/`, `.claude/`로 명시한다.
- `exec-plan-gate` skill이 올바른 저장 위치에 존재한다.
- `server/skills/` 아래 잘못 만든 skill 파일이 제거된다.

## 진행 상황

- [completed] 루트 문서 규칙 확인
- [in-progress] 위치 정정 및 문서 수정
- [pending] 잘못 생성한 server skill 제거
- [pending] 변경 검토

## 설계 결정 기록

- skill과 agent는 앱 디렉터리 아래가 아니라 저장소 루트의 `.codex/`와 `.claude/`에 둔다.
- 문서 저장 위치 규칙은 상위 문서 워크플로에서 관리한다.
- Codex 전용 UI 메타데이터는 `.codex/skills/<skill>/agents/openai.yaml`에 둔다.
