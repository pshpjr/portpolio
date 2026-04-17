---
name: architecture-eval
description: Evaluate architecture options whenever a proposal or design needs structure choices compared explicitly. This skill must be used when responsibilities, data ownership, boundaries, extensibility, or testability need a real option analysis rather than an implicit default.
---

# Architecture Eval

Use this skill when the team needs a deliberate structural choice.

## Primary Agent

- `architecture-evaluator`

## Workflow

1. Read the relevant proposal docs, design notes, and current task notes.
2. Extract or propose at least two architecture options.
3. Compare them across:
   - responsibility boundaries
   - data ownership and flow
   - change cost
   - testability
   - operational complexity
   - portfolio fit
4. Recommend one option and explain why the others are weaker.
5. Record any mitigation needed for the chosen option.

## Output Expectations

- Option comparison
- Recommendation
- Risks
- Mitigations
- Revisit triggers if the assumptions change
