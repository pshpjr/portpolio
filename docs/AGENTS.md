# AGENTS.md
> 루트 `docs/`용 에이전트 진입점입니다.
> 이 문서는 앱 구현 문서가 아니라 상위 기획과 공용 문서 라우팅만 담당합니다.

---

## 대상 범위

- `proposal/` — 제품 기획서
- `workflow/` — 루트 공용 작업 절차
- `exec-plans/` — 루트 레벨 실행 계획

---

## 작업 시작 순서

1. 이 파일을 읽는다.
2. 작업 대상이 기획서인지, 공용 워크플로인지, 루트 exec-plan인지 결정한다.
3. [exec-plans/active/INDEX.md](./exec-plans/active/INDEX.md)에서 관련 active plan을 확인하고, 없으면 새 plan을 먼저 만든다.
4. 필요한 하위 문서만 선택해서 읽는다.
5. 앱 구현 세부사항이 필요해질 때만 해당 앱의 `AGENTS.md`로 내려간다.

---

## 읽기 원칙

- `docs/` 작업에서는 상위 문서만 우선 읽는다.
- 서버 구현 규칙이 필요할 때만 `../server/AGENTS.md`와 그 하위 문서를 연다.
- 기획서는 허브 문서부터 읽고, 필요한 세부 주제만 추가로 연다.
- 컨텍스트 절약이 목적이므로 하위 문서를 한 번에 모두 읽지 않는다.

---

## 빠른 라우팅

- 기획서 전체 개요: [proposal/main.md](./proposal/main.md)
- 기획서 작업 진입점: [proposal/AGENTS.md](./proposal/AGENTS.md)
- 공용 작업 절차: [workflow/agent-workflow.md](./workflow/agent-workflow.md)
- 루트 활성 exec-plan: [exec-plans/active/INDEX.md](./exec-plans/active/INDEX.md)
- 서버 구현 문서로 이동: [../server/AGENTS.md](../server/AGENTS.md)

## Skill / Agent 위치 규칙

- 저장소 공용 skill은 `./../.codex/skills/` 와 `./../.claude/skills/` 아래에 둔다.
- 저장소 공용 agent 프롬프트는 `./../.codex/agents/` 와 `./../.claude/agents/` 아래에 둔다.
- 앱 디렉터리(`server/` 등) 아래에는 skill 또는 agent 저장소를 만들지 않는다.
- Codex 전용 UI 메타데이터는 `.codex/skills/<skill>/agents/openai.yaml`에 둔다.

---

## 기획서 읽기 규칙

- 제품 방향과 마일스톤만 필요하면 `proposal/main.md`만 읽는다.
- 도메인 세부 규칙이 필요할 때만 `proposal/common/`, `proposal/ingame/`, `proposal/outgame/`, `proposal/server/` 아래 개별 문서를 읽는다.
- 서버 설계 참고가 필요하더라도 구현 규칙의 단일 소스는 여전히 `server/` 하위 문서다.
