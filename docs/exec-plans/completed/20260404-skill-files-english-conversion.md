# Exec Plan: Skill Files English Conversion

**Date:** 2026-04-04
**Goal:** Convert all `.claude/skills/` Korean content to English. Korean-language output to end users is no longer required.

## 목표

`.claude/skills/`와 관련 참조 문서를 영어로 통일해,
하네스 원본 문서의 언어 일관성을 확보한다.

## Scope

### SKILL.md files
- [x] `.claude/skills/harness/SKILL.md`
- [x] `.claude/skills/codex-delegate/SKILL.md`
- [x] `.claude/skills/doc-governance/SKILL.md`
- [x] `.claude/skills/opencode-review/SKILL.md`
- [x] `.claude/skills/exec-plan-gate/SKILL.md` (mostly English, minor cleanup)

### harness/references/
- [x] `agent-design-patterns.md`
- [x] `skill-writing-guide.md`
- [x] `skill-testing-guide.md`
- [x] `orchestrator-template.md`
- [x] `team-examples.md`
- [x] `qa-agent-guide.md`

### doc-governance/references/
- [x] `document-roles.md`
- [x] `review-checks.md`
- [x] `commands.md`

### .codex wrappers
- [x] `.codex/skills/doc-governance/SKILL.md` description sync if needed

## Rules
- Preserve all structure, section headings, tables, and code blocks exactly
- Translate prose and comments to natural English — do not transliterate awkwardly
- Keep YAML frontmatter `name` and `source` fields unchanged
- `description` field: already updated for harness, codex-delegate, exec-plan-gate — re-check alignment
- `.codex` reference-only wrappers need description field sync only

## Progress
- Parallel worker delegation: 4 batches

## 완료 기준

- [x] `.claude/skills/` 대상 파일이 영어로 변환된다.
- [x] 지정된 `references/` 문서가 영어로 변환된다.
- [x] `.codex` 참조 래퍼의 description이 원본과 정합성을 유지한다.

## Validation

- 2026-04-05: repository scan confirmed no Korean text remains under the targeted `.claude/skills/**` and `.codex/skills/**` files except intentionally retained non-target docs.
