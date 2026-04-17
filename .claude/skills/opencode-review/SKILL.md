---
name: opencode-review
description: Delegate a review or inspection task to the local OpenCode CLI through headless `opencode run` for a second opinion. Use after Claude's own review, or when an independent perspective is needed on code, documentation, or proposal content.
---

# OpenCode Review Delegation Skill

Request an additional review from the local OpenCode CLI using the project's headless agent setup.
Use this skill to obtain a second perspective and reduce blind spots.

## When to Use

- When an independent second opinion is needed before a large PR or architecture change
- When additional consistency validation is needed after a documentation restructure
- When examining planning decisions from multiple angles for logical flaws
- When Claude has finished a review but confidence is low

## Basic Workflow

1. **Claude goes first** — complete a primary analysis using its own review or a relevant agent (reviewer, docs-reviewer, etc.).
2. **Compile the list of files to review and the relevant context.**
3. **Invoke via `review_delegate.sh` wrapper** (foreground, blocking).

```bash
bash tools/review_delegate.sh opencode "[review prompt]"
```

The wrapper uses `opencode run --format json --agent portpolio-review` internally and extracts only the final text response — no intermediate tool calls, no ANSI noise.

4. **Combine the OpenCode result with Claude's result** to reach a final conclusion.

### Parallel execution

Multiple Bash tool calls in a single message will run in parallel, each blocking independently:

```
Bash 1: bash tools/review_delegate.sh codex "서버 관점 리뷰..."
Bash 2: bash tools/review_delegate.sh opencode "클라이언트 관점 리뷰..."
```

Both return clean results directly in the tool response — no polling, no extra reads.

## Example Invocation

```bash
bash tools/review_delegate.sh opencode "
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

- OpenCode and Claude agree → high-confidence conclusion
- Only OpenCode finds an issue → that finding must be re-examined
- Only Claude finds an issue → retain it (OpenCode not finding it is not a refutation)
- They conflict → present both perspectives to the user and ask for a judgment call

## After Review: Clear Fix Path

When the review produces a clear fix direction, delegate immediately to OpenCode direct-apply mode.

| Situation | Path |
|-----------|------|
| Fix scope is ambiguous or requires design judgment | Claude fixes directly |
| Target files and changes are clear, and OpenCode has sole write ownership | OpenCode direct-apply |

### OpenCode Direct-Apply Delegation

```bash
opencode run --agent portpolio-implement --format json "[fix task prompt]"
```

- The prompt must specify target file paths, fix details, and verification method.
- After completion, Claude reviews the result and updates task/work notes.
- When write ownership is not isolated, use proposal mode (`portpolio-propose`) instead.

## Notes

- The repository does not pin an OpenCode model. `opencode run` uses the local OpenCode provider/model configuration.
- If `opencode` is not installed or the headless run fails, skip this skill and report the reason.
- OpenCode results are a second opinion. They do not replace Claude's judgment.
- Review results do not need to be recorded in task/work notes (volatile review opinions should not live in persistent scope notes).
