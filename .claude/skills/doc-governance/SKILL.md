---
name: doc-governance
description: Use this skill when reviewing, updating, or validating repository documentation structure, document ownership, context-budget rules, link consistency, or agent-facing entrypoint documents such as AGENTS.md, INDEX.md, context-map.md, workflow docs, and architecture docs.
---

# Doc Governance

Use this skill to keep documentation "short, role-separated, and read only as much as agents need."

## When to Use This Skill

- When modifying `AGENTS.md`, `INDEX.md`, `context-map.md`, `ARCHITECTURE.md`, or `workflow/*.md`
- When restructuring documentation, removing duplication, or improving routing
- When reviewing documentation PRs for role violations, duplication, stale links, or context bloat
- When designing doc inspection scripts or CI rules

## Basic Workflow

1. Look at the changed path and verify the document's role in [references/document-roles.md](./references/document-roles.md).
2. Before editing or reviewing, apply the checklist in [references/review-checks.md](./references/review-checks.md).
3. After modifying any document, run `python tools/doc_check.py` from [references/commands.md](./references/commands.md) before marking the work done.
4. If needed, select additional inspection commands from the same file.
5. Use a project sub-agent for a more narrowly focused review.

## Project Sub-Agents

- `docs-reviewer`
- `docs-structure-reviewer`
- `docs-consistency-reviewer`
- `docs-context-reviewer`

## Output Principles

- When a problem is found, state "which document is violating which role" first.
- Propose improvements as structural actions — moving, splitting, or adding links — not prose rewrites.
- Do not re-summarize document contents at length; judge only roles and boundaries.
