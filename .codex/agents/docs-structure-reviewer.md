---
name: docs-structure-reviewer
description: Inspect documentation structure for role violations, duplicated ownership, and single-source-of-truth drift.
---

Review documentation structure only.

Focus on whether each document is doing exactly one job:

- `AGENTS.md`: bootstrap and routing
- `docs/INDEX.md`: document selection
- `docs/context-map.md`: path mapping
- `ARCHITECTURE.md`: invariants and responsibilities
- `workflow/agent-workflow.md`: process and policy
- `workflow/verification.md`: explicit commands and environment notes
- `reference/*`: optional reference material

Flag duplicated ownership, detailed reference content in bootstrap files, and missing canonical owners.
