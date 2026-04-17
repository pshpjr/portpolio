---
name: codex-delegate
description: >
  Delegate implementation tasks to Codex and OpenCode headless agents to save Claude context and
  maximize output quality. Claude decides WHAT to implement; Codex (`codex exec`) and OpenCode
  (`opencode run`) can both work on the task, and Claude chooses the correct path based on whether
  the write scope is shared or isolated. Use when: C++ scaffolding, batch file generation, running
  code-gen scripts (e.g. generate_*.py), build execution, or any file creation / modification task.
  Do NOT use for design decisions or user coordination — Claude handles those directly.
---

# Parallel Implementation Executor

Claude's role is to decide **what** to implement and write a clear task prompt. File writing can be
delegated to Codex and OpenCode headless agents. Use OpenCode proposal mode when you need
side-by-side comparison, and use direct apply mode only when OpenCode has isolated write ownership.

## Execution Model

```
Claude (decides scope + writes task prompt)
    ├── [parallel] codex exec  ──────────────────────────────→ writes files directly → output A
    └── [parallel] opencode run --agent portpolio-propose ─→ returns proposed code/text → output B
                                                         ↓
                                        Claude compares A vs B → select or merge
```

## Step 1: Write a Self-Contained Task Prompt

Before delegating, write a prompt that either executor can run independently:

```
□ Which files to create or modify (exact paths)
□ Context files to read (AGENTS.md, task note, ARCHITECTURE.md, etc.)
□ Rules to follow (layer rules, naming conventions, style guide)
□ Output format and file paths
□ How to verify completion (build, lint, check script)
```

## Step 2: Run Codex and OpenCode

### Codex — Invocation Syntax

| Scenario | Command |
|----------|---------|
| Read-only investigation | `codex exec --sandbox read-only --ask-for-approval never "[prompt]"` |
| File creation / editing (default) | `codex exec --sandbox workspace-write --ask-for-approval untrusted "[prompt]"` |
| Fully unrestricted (use sparingly) | `codex exec --yolo "[prompt]"` |
| Specify working directory | `codex exec -C D:\path\to\repo "[prompt]"` |
| Short alias | `codex e "[prompt]"` |

Default for most implementation tasks:
```bash
codex exec --sandbox workspace-write --ask-for-approval untrusted "[task prompt]"
```

### OpenCode Headless — Proposal Mode

Use the identical task prompt with the project's non-writing OpenCode agent:

```bash
opencode run --agent portpolio-propose --format json "[task prompt — identical to what was sent to Codex]"
```

This is the safe default when Codex is writing to the same files.

### OpenCode Headless — Direct Apply Mode

If OpenCode should edit files directly instead of returning a proposal:

```bash
opencode run --agent portpolio-implement --format json "[task prompt]"
```

Use the direct-apply path only when OpenCode owns a disjoint write scope or is running in an isolated worktree.

## Step 3: Compare and Select

After both complete, evaluate on these criteria:

| Criterion | What to check |
|-----------|---------------|
| **Correctness** | Layer rules, type consistency, no obvious bugs |
| **Completeness** | All required files and sections present |
| **Convention** | Matches project style (cpp-style.md, naming rules, etc.) |
| **Diff scope** | Smaller, targeted changes preferred over unnecessary rewrites |

**Decision rules:**

| Situation | Action |
|-----------|--------|
| Both agree | Either is fine — Codex output is already on disk, use it |
| Only Codex is correct | Codex output already applied, done |
| Only OpenCode proposal is correct | Apply or merge OpenCode's proposed text |
| Both partially correct | Merge best parts — Claude applies the merged version |
| Both wrong | Re-delegate with a refined prompt |

## Step 4: Validate

After applying the selected output:
- Server work: run `check_deps.py`
- Documentation work: run `doc_check.py`
- Update task/work notes progress
- Report any remaining issues

## What Claude Delegates vs. Handles Directly

**Always delegate (when the scope fits):**
- File creation / scaffolding
- Code-gen script execution
- Repetitive edits and bulk renames
- Build execution
- Independent file-level investigation
- OpenCode direct-apply runs with isolated write ownership

**Claude handles directly (never delegate):**
- Design decisions and architectural trade-offs
- Writing or reviewing task/work notes
- Agent / skill architecture decisions
- Incorporating user feedback
- Layer boundary judgments requiring full context

## References

Delegation protocol examples: `references/delegation-examples.md` (create if needed)
