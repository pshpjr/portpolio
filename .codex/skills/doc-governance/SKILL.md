---
name: doc-governance
description: Use this skill when reviewing, updating, or validating repository documentation structure, document ownership, context-budget rules, link consistency, or agent-facing entrypoint documents such as AGENTS.md, INDEX.md, context-map.md, workflow docs, and architecture docs.
---

# Doc Governance

이 스킬은 문서가 "짧고, 역할이 분리되고, 에이전트가 필요한 만큼만 읽도록" 유지하는 데 사용한다.

## 언제 이 스킬을 사용할까

- `AGENTS.md`, `INDEX.md`, `context-map.md`, `ARCHITECTURE.md`, `workflow/*.md`를 수정할 때
- 문서 구조 개편, 문서 중복 제거, 라우팅 개선을 할 때
- 문서 PR 리뷰에서 역할 위반, 중복, stale link, 컨텍스트 비대화를 점검할 때
- 문서 검사 스크립트나 CI 규칙을 설계할 때

## 기본 워크플로

1. 변경 경로를 보고 문서 역할을 [references/document-roles.md](./references/document-roles.md)에서 확인한다.
2. 수정 또는 리뷰 전에 [references/review-checks.md](./references/review-checks.md)의 체크리스트를 적용한다.
3. 필요한 검사 커맨드는 [references/commands.md](./references/commands.md)에서 고른다.
4. 더 좁은 관점의 검토가 필요하면 아래 하위 에이전트 프롬프트를 사용한다.

## 하위 에이전트

- `agents/docs-reviewer.md`: 문서 PR의 총괄 리뷰어
- `agents/docs-structure-reviewer.md`: 문서 역할 분리와 단일 소스 위반 검사
- `agents/docs-consistency-reviewer.md`: 링크, 경로, 인덱스, 참조 일관성 검사
- `agents/docs-context-reviewer.md`: 진입 문서의 길이와 선독 범위 비대화 검사

## 출력 원칙

- 문제를 찾으면 "어느 문서가 어떤 역할을 침범했는지"를 먼저 적는다.
- 개선 제안은 문서 이동, 분리, 링크 추가처럼 구조적 조치로 제안한다.
- 문서 내용을 길게 재서술하지 말고 역할과 경계만 판단한다.
