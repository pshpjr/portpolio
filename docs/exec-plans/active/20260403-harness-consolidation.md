# Harness Consolidation: Single Source of Truth

**목표**: `.claude/`를 원본 소스, `.codex/`를 참조 래퍼로 확립. 누락 폴더 추가.

## 현재 문제

| 항목 | `.claude/` | `.codex/` | 문제 |
|---|---|---|---|
| agents | 15개 (풀 정의) | 7개 (빈 스텁) | 8개 누락, 내용 중복 |
| skills | 4개 (풀 정의) | 2개 (중복 내용) | 2개 누락, references/ agents/ 중복 |

## 작업 범위

### 1. `.codex/agents/` — 누락 에이전트 추가 + 모두 최소 참조 스텁으로 통일
- 기존 7개: 1줄 canonical reference로 교체
- 추가 8개: reviewer, docs-reviewer, docs-structure-reviewer, docs-consistency-reviewer, docs-context-reviewer, codegen-specialist, design-author, server-implementer

### 2. `.codex/skills/` — 중복 내용 제거 + 누락 스킬 추가
- `doc-governance/`: SKILL.md→참조 래퍼, `agents/` 제거, `references/` 제거, `openai.yaml` 유지
- `exec-plan-gate/`: SKILL.md→참조 래퍼, `openai.yaml` 유지
- 추가: `harness/` (openai.yaml + stub SKILL.md)
- 추가: `codex-delegate/` (openai.yaml + stub SKILL.md)

### 3. CLAUDE.md 빠른 참조에 정책 명시
- `.claude/` = 원본 소스
- `.codex/` = 참조 래퍼만

## 완료 기준

- `.codex/agents/` 15개 전체 (`.claude/agents/`와 1:1 대응)
- `.codex/skills/` 4개 전체 (`.claude/skills/`와 1:1 대응)
- 중복 파일 없음 (doc-governance/agents/, doc-governance/references/ 제거)

## 상태

- [x] `.codex/agents/` 정리 (15개 최소 참조 스텁)
- [x] `.codex/skills/doc-governance/` 정리 (SKILL.md 래퍼, agents/refs 참조 스텁)
- [x] `.codex/skills/exec-plan-gate/` 정리 (SKILL.md 래퍼)
- [x] `.codex/skills/harness/` 추가 (SKILL.md + openai.yaml)
- [x] `.codex/skills/codex-delegate/` 추가 (SKILL.md + openai.yaml)
- [x] CLAUDE.md 정책 추가

## 확장: OpenCode 리뷰어 통합

**목표**: http://127.0.0.1:4096 에서 실행 중인 OpenCode를 추가 리뷰어로 연동.

- [x] `.claude/agents/opencode-reviewer.md` 생성 (API 호출 에이전트)
- [x] `.claude/skills/opencode-review/SKILL.md` 생성 (리뷰 위임 스킬)
- [x] `.codex/agents/opencode-reviewer.md` 추가 (참조 스텁)
- [x] `.codex/skills/opencode-review/` 추가 (참조 래퍼 + openai.yaml)
- [x] `agent-workflow.md` — OpenCode 리뷰 단계 추가
- [x] `agent-workflow.md` — `.codex/agents/` 정책 설명 수정
