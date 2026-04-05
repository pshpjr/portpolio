---
name: office-hours
description: Run the top-level office-hours planning loop for portpolio whenever the user gives a rough feature idea, a product question, or asks the harness to figure out planning automatically from one prompt. This skill must be used when one request should expand proposal docs, route into separated review skills, and end with an OpenCode(GLM) cross-review.
---

# Office-Hours

Use this skill as the top-level planning orchestrator.
It turns one fuzzy input into a structured proposal round.

## When to Use

- The user gives a feature idea and wants the harness to shape planning automatically
- The user wants a single office-hours pass that fans out into other reviews
- The task should update `docs/proposal/**` rather than jump straight into code

## Do Not Use

- For narrow single-file edits
- For direct code implementation in `server/` or `Lib/`
- For browser QA or deployment work

## Workflow

1. Run `exec-plan-gate` first.
2. Read `docs/workflow/gstack-office-hours-loop.md`.
3. Use `office-hours-orchestrator` to define scope, target docs, and required analysis tracks.
4. Have `design-author` create or backfill the proposal draft.
5. Route into the separated skills that fit the round:
   - `plan-ceo-review`
   - `plan-eng-review`
   - `architecture-eval`
   - `performance-analysis`
   - `tradeoff-resolution`
6. Feed the resulting gaps and recommendations back into proposal docs.
7. Use `opencode-review` for the final OpenCode/GLM cross-review.
8. Leave only true unresolved items in `docs/proposal/pending-decisions.md`.

## Output Expectations

- Proposal docs are updated, not just review notes
- The run shows which analysis tracks were executed
- GLM disagreements are called out explicitly
- Web-only `gstack` flows stay out of the run
