# Exec Plan: 하네스 단일 소스 정리

## 목표

에이전트 정의 파일의 중복을 제거하여 `.claude/agents/`를 단일 소스로 확립하고,
관련 문서의 규칙과 중복 내용을 정리한다.

## 범위

- `.codex/agents/` — 7개 파일 삭제 (단일 소스에서 제외)
- `docs/workflow/agent-workflow.md` — 피드백 로그 섹션 중복 3개 제거, Skill/Agent 위치 규칙 수정
- `docs/workflow/exec-plan-template.md` — 신규 생성 (템플릿 문서화)
- `docs/AGENTS.md` — Skill/Agent 위치 규칙 수정
- `docs/exec-plans/active/INDEX.md` — 이 plan 등록

## 완료 기준

- [x] `.codex/agents/` 내 파일 7개 삭제 완료
- [x] `agent-workflow.md` 피드백 로그 섹션이 1개만 남음
- [x] `agent-workflow.md` Skill/Agent 규칙: `.claude/agents/`가 단일 소스로 명시, `.codex/agents/`에 생성 금지 명시
- [x] `docs/AGENTS.md` 동일 규칙 반영
- [x] `exec-plan-template.md` 생성 완료
- [x] `agent-workflow.md`에 템플릿 문서 링크 추가

## 진행 상황

- [x] exec-plan 작성
- [x] `exec-plan-template.md` 생성
- [x] INDEX.md 등록
- [x] `.codex/agents/` 파일 삭제
- [x] `agent-workflow.md` 정리
- [x] `docs/AGENTS.md` 규칙 수정

## 설계 결정

- `.claude/agents/`를 단일 소스로 선택한 이유: Claude Code가 주 실행 환경이며, git clone 후 심볼릭 링크 없이 바로 동작해야 한다.
- Codex에서 에이전트를 사용할 때는 `.claude/agents/<name>.md`를 읽어 prompt로 전달한다.
- `.codex/agents/`는 삭제. `.codex/skills/<skill>/agents/openai.yaml`은 Codex UI 전용 메타데이터이므로 유지.
- exec-plan 전용 템플릿 문서를 `docs/workflow/`에 추가. agent-workflow.md에 인라인으로 있던 템플릿은 링크로 대체.
