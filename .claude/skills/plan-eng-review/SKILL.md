---
name: plan-eng-review
description: Run the engineering planning review whenever proposal docs are about to become implementation work or when the user wants server/client/data-flow gaps surfaced early. This skill must be used when you need state transitions, data contracts, failure paths, ownership rules, and implementation-facing missing details identified before coding starts.
---

# Plan ENG Review

This skill reviews whether the proposal actually gives implementation teams enough contracts to work from.

## Primary Agents

- `server-proposal-critic`
- `client-proposal-critic`

## Workflow

1. Read `docs/proposal/AGENTS.md`, the relevant proposal docs, and the active plan.
2. Ask both critics to inspect the same scope from their own angle.
3. Merge the findings into a single implementation-facing gap list.
4. Translate every gap into proposal language:
   - missing state
   - missing field
   - missing rule
   - missing failure behavior
5. Route the accepted gaps back into `design-author`.
6. Keep architecture, performance, and tradeoff deep-dives in their own specialized skills unless a lightweight mention is enough.

## Output Expectations

- Server-side missing contracts
- Client-side missing contracts
- Proposal backfill requests
- Items that must move to `pending-decisions.md`
