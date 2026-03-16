---
name: exec-plan-gate
description: Require an exec-plan before starting any repository work. Use when a user asks for implementation, refactoring, documentation, tests, scripts, build changes, or any other task so Codex first finds or creates the relevant exec-plan, updates the active plan index, asks the user to review the plan, and waits before editing implementation files.
---

# Exec Plan Gate

1. Read the relevant area entrypoint first:
   - `docs/AGENTS.md` for root documentation work
   - `server/AGENTS.md` for server work
2. Read the matching active exec-plan index and determine whether an existing plan already covers the task.
3. If no matching plan exists, create a new exec-plan in the correct `docs/exec-plans/active/` tree.
4. Update the active index to link the new plan.
5. Summarize the plan to the user and ask for review before touching implementation files.
6. Do not edit code, tests, scripts, build files, or non-plan documents until the user responds to the review step.
7. If the user asks to adjust scope or approach, update the plan first and ask for review again.
8. After user approval, implement the task and keep the plan progress current.
9. When work finishes, update the plan and move it to `completed/` if the local workflow requires it.

Treat plan creation or confirmation as mandatory for every task in this repository.
