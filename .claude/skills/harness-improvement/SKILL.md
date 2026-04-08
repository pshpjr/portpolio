---
name: harness-improvement
description: Register or execute recursive harness improvements whenever a task exposes reusable friction in workflow docs, prompts, skills, or tooling. This skill must be used when leaving a follow-up item in `docs/workflow/harness-improvement-queue.md`, when tracking whether a script or skill could reduce context usage, and when `harness-improver` claims one queued item to fix it.
---

# Harness Improvement

Use this skill to keep process problems out of chat history and inside repository-managed follow-up work.

## When to Use

- A task uncovered repeatable workflow or tooling friction
- A prompt or skill gap forced a workaround that should be fixed later
- A queued improvement should be claimed and executed by another agent
- Repeated context loading suggests that a script or a new skill could reduce token usage

## Core Split

All improvements go directly to the queue:

- `docs/workflow/harness-improvement-queue.md` records independent follow-up work with full context

If the issue should cause an actual repository change later, it belongs in the queue.

## Queue Rules

Every queued item must include:

- `task`
- `message`
- `recommended-artifact`
- `context-savings`
- `token-meter` when measured, otherwise `pending`

Use `recommended-artifact: script` when deterministic repeated work should be bundled into code.
Use `recommended-artifact: skill` when instructions or routing are being re-explained across runs.

## Token Meter

When the task is substantial or when optimization is being considered, measure the context load with:

```bash
python tools/context_meter.py --git-base HEAD --files <context-file-1> <context-file-2> ...
```

Record the result in the active exec-plan or the queued item so future agents can compare work size against estimated token load.

## Response Artifacts

After an external LLM or OpenCode response, do not persist raw hidden thinking.
Persist a short structured artifact instead:

```bash
python tools/record_agent_artifact.py --agent <agent> --summary "<summary>" --source "<task or command>"
```

If the response reveals an independent follow-up item, include `--task` and `--message` so it is appended to the queue automatically.

## Workflow

1. Run `exec-plan-gate` first if the improvement itself becomes repository work.
2. Read the feedback log, improvement queue, and relevant workflow docs.
3. Decide whether you are:
   - registering a new follow-up item, or
   - claiming and executing an existing item
4. If registering:
   - add one item to `Open Tasks`
   - write a short imperative `task`
   - write a short explanatory `message`
   - capture whether `script` or `skill` would reduce repeated context
5. If executing:
   - claim one item
   - make the smallest safe fix
   - update the queue item status when done
6. If the fix changes repository workflow, prompts, or docs, update the canonical file instead of leaving a permanent queue workaround.

## Output Expectations

- The queue stays short and actionable
- Follow-up work is understandable without replaying the full chat
- Script and skill opportunities are visible, not implicit
- Completed items are reflected in repository docs, prompts, or tools
