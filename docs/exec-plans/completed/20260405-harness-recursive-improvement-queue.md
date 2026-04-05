# Exec Plan: Harness Recursive Improvement Queue

## 목표

작업 중 발견된 하네스 결함과 개선 후보가 채팅에서 소실되지 않도록,
재사용 가능한 후속 작업을 `task` / `message` 형식 큐로 남기고 다른 에이전트가 집어가게 하며,
완료된 작업은 후속 수정 전에 먼저 커밋해 AI 작업 이력을 비교 가능하게 남긴다.

## 범위

- `docs/exec-plans/active/INDEX.md`
- `docs/workflow/agent-workflow.md`
- `docs/workflow/harness-feedback-log.md`
- `docs/workflow/harness-improvement-queue.md` 신규
- `.claude/agents/harness-improver.md` 신규
- `.codex/agents/harness-improver.md` 신규 참조 스텁
- `.claude/skills/harness-improvement/SKILL.md` 신규
- `.codex/skills/harness-improvement/SKILL.md` 신규 참조 래퍼
- `.codex/skills/harness-improvement/agents/openai.yaml` 신규
- `.claude/skills/harness/SKILL.md`
- `tools/context_meter.py` 신규
- `tools/record_agent_artifact.py` 신규
- `docs/conventions/tooling.md`

## 완료 기준

- [x] 작업 중/작업 완료 시 개선 항목을 남길 큐 문서가 추가된다
- [x] 큐 항목 형식에 `task`와 `message`가 필수로 정의된다
- [x] 큐 항목이 스크립트/스킬화 가능성과 컨텍스트 절감 효과를 추적할 수 있다
- [x] 다른 에이전트가 큐 항목을 claim 하고 처리할 전용 agent/skill이 추가된다
- [x] 공용 workflow 문서가 로그와 작업 큐의 역할 차이를 설명한다
- [x] 작업 완료 후 후속 수정 전에 먼저 커밋하는 추적성 규칙이 workflow에 명시된다
- [x] 작업 분량 대비 컨텍스트/토큰 사용량을 계측하는 공용 도구와 기록 규칙이 추가된다
- [x] 외부 LLM/OpenCode 응답 직후 구조화된 응답 노트를 파일로 남기고, 필요 시 큐에 자동 추가하는 경로가 정의된다
- [x] `python tools/doc_check.py`가 통과한다

## 진행 상황

- [x] 관련 workflow/log 구조를 검토하고 필요한 산출물을 결정했다
- [x] 개선 큐 문서와 workflow 규칙을 추가한다
- [x] 스크립트/스킬 후보와 컨텍스트 절감 효과를 큐에서 추적하게 한다
- [x] `harness-improver` agent/skill을 추가한다
- [x] 자동 커밋 추적성 규칙을 workflow에 명시한다
- [x] 토큰 계측 도구와 응답 아티팩트 기록 도구를 추가한다
- [x] 문서 검증 후 남은 실제 개선 항목이 있으면 초기 큐 항목으로 남긴다

## 설계 결정

- 피드백 로그와 작업 큐를 분리한다. 로그는 회고/패턴 기록, 큐는 바로 처리 가능한 후속 작업만 담는다.
- 큐 항목은 최소한 `task`와 `message`를 포함한다. `task`는 다음 에이전트가 수행할 명령형 작업, `message`는 왜 필요한지와 재현 맥락을 남긴다.
- 큐 항목은 가능하면 "문서 수정"만 볼 것이 아니라, 스크립트 추가나 새 스킬 생성이 컨텍스트/토큰 비용을 줄일지도 함께 추적한다.
- 다른 에이전트가 처리하도록 전용 `harness-improver` agent를 둔다. 이렇게 해야 "누가 처리하는가"가 명확해지고, 메인 작업자는 개선 아이템만 남기고 본 작업을 끝낼 수 있다.
- 후속 수정 요청이 오더라도 직전 완료 상태를 먼저 커밋한다. 그래야 다음 수정이 이전 AI 산출물을 덮어써도 비교 기준이 남는다.
- raw thinking 저장은 하지 않고, 외부 LLM 응답 직후의 구조화된 요약/결정/후속 작업/토큰 계측만 파일에 남긴다.

## 검증 기록

- `python tools/context_meter.py --git-base HEAD --files docs/workflow/agent-workflow.md docs/workflow/harness-improvement-queue.md docs/workflow/harness-feedback-log.md docs/conventions/tooling.md .claude/skills/harness/SKILL.md .claude/skills/harness-improvement/SKILL.md tools/context_meter.py tools/record_agent_artifact.py`
  - `estimated_context_tokens: 11538`
  - `changed_lines: 825`
  - `estimated_tokens_per_changed_line: 13.99`
  - 참고: untracked 새 파일도 작업 분량에 포함하도록 `context_meter.py`를 보정했다
- `python tools/record_agent_artifact.py ...` → `_workspace/agent-notes/20260405-151124-harness-improver.md` 생성 및 개선 큐 자동 추가 확인
- `python tools/doc_check.py` → `OK: documentation checks passed.`
- 참고: 기존 active plan `20260404-office-hours-proposal-polish.md`에 `목표` 섹션 경고 1건이 남아 있었으며, 이는 개선 큐의 open task로 남겼다. (해당 파일은 이후 삭제됨)
