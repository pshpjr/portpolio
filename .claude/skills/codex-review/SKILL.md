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
3. **Invoke via `review_delegate.sh` wrapper** (foreground, blocking).

```bash
bash tools/review_delegate.sh codex "[review prompt]"
```

The wrapper uses `codex exec --sandbox read-only --ephemeral -o` internally and returns only the final review text — no intermediate logs, no PowerShell noise.

4. **Combine the Codex result with Claude's result** to reach a final conclusion.

### Parallel execution

Multiple Bash tool calls in a single message will run in parallel, each blocking independently:

```
Bash 1: bash tools/review_delegate.sh codex "서버 관점 리뷰..."
Bash 2: bash tools/review_delegate.sh opencode "클라이언트 관점 리뷰..."
```

Both return clean results directly in the tool response — no polling, no extra reads.

## Example Invocation

```bash
bash tools/review_delegate.sh codex "
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