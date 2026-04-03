# Auto Commit Push Workflow

## 목표
작업 완료 후 검증이 끝나면 관련 변경분을 자동으로 커밋하고 푸시하도록 공용 workflow와 agent 프롬프트를 정렬한다.

## 범위
- `docs/exec-plans/active/INDEX.md`
- `docs/exec-plans/active/20260330-auto-commit-push-workflow.md`
- `docs/workflow/agent-workflow.md`
- `docs/workflow/harness-feedback-log.md`
- `.codex/agents/worker.md`
- `.codex/agents/scaffolder.md`
- `.claude/agents/worker.md`
- `.claude/agents/scaffolder.md`

## 완료 기준
- [x] 공용 workflow 문서에 완료 후 자동 commit/push 규칙이 추가된다.
- [x] Codex/Claude 공용 worker/scaffolder 프롬프트가 같은 규칙을 따른다.
- [x] 자동화 규칙에 안전 조건과 예외가 포함된다.
- [x] 사용자 요청을 피드백 로그에 남긴다.
- [x] 문서 검증 결과를 기록한다.
- [ ] 변경분만 선별 커밋한다.

## 진행 상황
- [x] `docs/AGENTS.md`, `docs/workflow/agent-workflow.md`, `.codex/.claude` agent 프롬프트를 확인했다.
- [x] root workflow와 agent 문서에 자동 commit/push 규칙을 반영했다.
- [x] 피드백 로그를 남겼다.
- [x] 문서 검증을 실행했다.
- [ ] 변경분만 선별 커밋한다.

## 설계 메모
- 자동 커밋/푸시는 "검증 통과 후"와 "내 작업만 선별 스테이징"이 같이 있어야 안전하다.
- dirty worktree, push remote 부재, 브랜치 권한 문제처럼 안전한 자동 푸시가 불가능한 경우는 강행보다 blocker 보고가 맞다.
- 실제 동작 규칙은 workflow 문서와 worker/scaffolder 프롬프트 양쪽에 있어야 세션별 해석 차이가 줄어든다.
- `docs/workflow/agent-workflow.md`에는 검증 통과 후 선별 스테이징, 커밋, 현재 또는 합의된 브랜치로 푸시하는 기본 순서를 추가했다.
- `.codex/.claude`의 `worker.md`, `scaffolder.md`에는 자동 commit/push 책임과 안전한 선별 스테이징 예외 규칙을 추가했다.
- `python tools/doc_check.py` 실행 결과 문서 검증은 통과했다.
