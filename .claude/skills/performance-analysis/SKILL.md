---
name: performance-analysis
description: Analyze performance early whenever a proposal, architecture, or implementation plan carries meaningful throughput, latency, memory, network, or concurrency risk. This skill must be used when "we should measure this" is true but the measurements, bottlenecks, and acceptance criteria have not been written down yet.
---

# Performance Analysis

Use this skill to turn vague performance concern into an explicit measurement plan.

## Primary Agent

- `performance-analyst`

## Workflow

1. Read the relevant proposal docs, design notes, and current task notes.
2. Identify the highest-risk performance paths.
3. For each path, define:
   - bottleneck hypothesis
   - load or stress scenario
   - metrics to capture
   - rough acceptance target
   - what to instrument later
4. Distinguish must-have performance guarantees from premature optimization.
5. Write the result as a testable analysis artifact, not a hunch list.

## Output Expectations

- Bottleneck hypotheses
- Metrics and acceptance criteria
- Load scenarios
- Instrumentation points
- Priority order
