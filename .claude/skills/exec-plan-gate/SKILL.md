---
name: exec-plan-gate
description: Before any repository work, find or create the relevant active exec-plan, update the active index, summarize the current scope briefly, and keep the plan current while working. Use the queue only for follow-up work discovered during implementation.
---

# Exec Plan Gate

All repository work requires an exec-plan before implementation.

## Applies to
Implementation, refactoring, documentation, tests, scripts, build/config changes, and any other repository edits.

## Start

1. Read the entry doc for the target area:
   - `docs/AGENTS.md` for root/docs work
   - `server/AGENTS.md` for server work

2. Read `docs/exec-plans/active/INDEX.md`.

3. Find the active exec-plan for the current task.
   - If a matching active plan exists, reuse and update it.
   - Otherwise create a new plan in `docs/exec-plans/active/`.

4. Update `docs/exec-plans/active/INDEX.md`.

5. Briefly summarize the plan and current scope to the user.

6. Unless the user explicitly asks to review first, begin implementation immediately.

## During work

7. If scope or approach changes, update the active plan first, then continue.

8. If you discover useful follow-up work that is outside the current agreed scope and not required to finish the current task, ask whether to queue it or handle it now.
   - If queued:
      1. Read `docs/workflow/queue-item-template.md`
      2. Create `docs/exec-plans/queued/YYYYMMDD-작업명.md`
      3. Update `docs/exec-plans/queued/INDEX.md`

9. If implementation reveals execution errors, repeated manual fixes, or work better extracted into a reusable script or module, queue it unless it must be resolved immediately to complete the current task.

10. Keep the active plan current while implementing.

## Finish

11. When the task is complete:
- update the final status
- move the plan to `docs/exec-plans/completed/`
- update indexes accordingly

## Claiming queued work

1. Pick an item from `docs/exec-plans/queued/INDEX.md`
2. Move it to `docs/exec-plans/active/`
3. Remove it from `queued/INDEX.md`
4. Add it to `active/INDEX.md`
5. Expand it into a full exec-plan