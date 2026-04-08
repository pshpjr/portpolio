---
name: codex-review
description: Delegate a review or inspection task to the local Codex CLI through headless `codex exec --sandbox read-only` for a second opinion. Use after Claude's own review, or when an independent perspective is needed on code, documentation, or proposal content. Symmetric to opencode-review.
---

# Codex Review Delegation Skill

Request an additional review from the local Codex CLI using read-only sandbox mode.
Use this skill to obtain a second perspective and reduce blind spots.

## When to Use

- When an independent second opinion is needed before a large PR or architecture change
- When additional consistency validation is needed after a documentation restructure
- When examining planning decisions from multiple angles for logical flaws
- When Claude has finished a review but confidence is low
- As an alternative to `opencode-review` when Codex is the preferred reviewer

## Basic Workflow

1. **Claude goes first** — complete a primary analysis using its own review or a relevant agent (reviewer, docs-reviewer, etc.).
2. **Compile the list of files to review and the relevant context.**
3. **Invoke Codex in read-only mode.**

```bash
codex exec --sandbox read-only --ask-for-approval never "[review prompt]"
```

4. **Combine the Codex result with Claude's result** to reach a final conclusion.

## Example Invocation

```bash
codex exec --sandbox read-only --ask-for-approval never "
Review type: Code review
Target file: server/src/03_core/combat/CombatSystem.cpp
Evaluation criteria:
  - Layer rule: 03_core must not depend on 04_service or 05_network
  - Single responsibility: check whether any class carries too many roles
  - Potential bugs: null checks, bounds checks, race conditions
Report issues as a numbered list with file:line references.
"
```

## Result Integration Principles

- Codex and Claude agree → high-confidence conclusion
- Only Codex finds an issue → that finding must be re-examined
- Only Claude finds an issue → retain it (Codex not finding it is not a refutation)
- They conflict → present both perspectives to the user and ask for a judgment call

## Notes

- `--sandbox read-only` ensures Codex makes no file writes during the review.
- If `codex` is not installed or the run fails, skip this skill and report the reason.
- Codex results are a second opinion. They do not replace Claude's judgment.
- Review results do not need to be recorded in the exec-plan (volatile review opinions should not live in the plan).