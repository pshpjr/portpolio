---
name: plan-ceo-review
description: Run a CEO-level planning review whenever proposal scope, product value, prioritization, or showcase clarity are in question. This skill must be used before committing to a broad feature set, when a proposal feels too large, or when the user wants a blunt answer about what to cut, defer, or sharpen.
---

# Plan CEO Review

This skill pressure-tests planning from the top down.
It is about value, focus, scope, and finishability.

## Primary Agent

- `portfolio-scope-guardian`

## Optional Supporting Agents

- `gameplay-loop-critic`
- `economy-and-progression-skeptic`
- `co-op-flow-advocate`

Only invoke supporting agents if their domain is central to the current request.

## Workflow

1. Read `docs/proposal/AGENTS.md` and `docs/proposal/main.md`.
2. Open only the relevant proposal docs and active plan.
3. Ask:
   - What does this add to the final portfolio story?
   - What should be cut or deferred?
   - Which promise is expensive but weak as showcase material?
   - Where is the scope still fuzzy?
4. Return explicit revision proposals with target document paths.
5. Route accepted changes back into `design-author`.

## Output Expectations

- Strongest showcase promise
- Top scope risks
- Concrete cuts, deferrals, merges, or sharpened scope proposals
