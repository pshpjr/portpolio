# CLAUDE.md

루트 하네스 진입점입니다.

## 시작 순서

1. `AGENTS.md`를 읽고 작업 영역을 결정한다.
2. 루트 문서 작업이면 `docs/AGENTS.md`를 읽는다.
3. 앱 구현이면 해당 앱 디렉터리의 `AGENTS.md`를 읽는다.
4. `docs/exec-plans/active/INDEX.md`에서 관련 exec-plan을 찾거나 새로 만든다.
5. plan 범위를 유지하면서 작업하고 진행 상황을 갱신한다.

## 루트 원칙

- 루트는 모노레포 조정과 공용 문서, 공용 skill/agent 관리가 우선이다.
- 앱 구현 세부 규칙은 각 앱 디렉터리 문서가 단일 소스다.
- exec-plan은 사용자 승인 대기용이 아니라 작업 맥락 유지와 범위 관리용으로 먼저 작성한다.
- 반복될 만한 사용자 피드백, 요청된 수정, 프로세스 이슈는 `docs/workflow/harness-feedback-log.md`에 짧게 남긴다.
- 저장소 공용 skill/agent 원본은 `.claude/skills/`, `.claude/agents/`에 둔다 (단일 소스).
- `.codex/skills/`, `.codex/agents/`는 참조 래퍼만 두며, 원본 내용을 복제하지 않는다.

## 빠른 참조

- 루트 진입: `AGENTS.md`
- 상위 문서 진입: `docs/AGENTS.md`
- 활성 exec-plan 인덱스: `docs/exec-plans/active/INDEX.md`
- 공용 작업 절차: `docs/workflow/agent-workflow.md`
