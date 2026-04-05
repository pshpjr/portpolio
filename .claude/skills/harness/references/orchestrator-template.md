# Orchestrator Skill Template

An orchestrator is a top-level skill that coordinates an entire team. Two templates are provided depending on the execution mode.

---

## Template A: Agent Team Mode (Default)

An agent team is formed with `TeamCreate`, and coordination is done through a shared task list and `SendMessage`.

```markdown
---
name: {domain}-orchestrator
description: "Orchestrator that coordinates the {domain} agent team. {trigger keywords}."
---

# {Domain} Orchestrator

An integrated skill that coordinates the {domain} agent team to produce {final output}.

## Execution Mode: Agent Team

## Agent Configuration

| Member | Agent Type | Role | Skill | Output |
|--------|-----------|------|-------|--------|
| {teammate-1} | {custom or built-in} | {role} | {skill} | {output-file} |
| {teammate-2} | {custom or built-in} | {role} | {skill} | {output-file} |
| ... | | | | |

## Workflow

### Phase 1: Preparation
1. Analyze user input — {what to determine}
2. Create `_workspace/` in the working directory
3. Save input data to `_workspace/00_input/`

### Phase 2: Team Assembly

1. Create the team:
   ```
   TeamCreate(
     team_name: "{domain}-team",
     members: [
       { name: "{teammate-1}", agent_type: "{type}", model: "opus", prompt: "{role description and task instructions}" },
       { name: "{teammate-2}", agent_type: "{type}", model: "opus", prompt: "{role description and task instructions}" },
       ...
     ]
   )
   ```

2. Register tasks:
   ```
   TaskCreate(tasks: [
     { title: "{task-1}", description: "{details}", assignee: "{teammate-1}" },
     { title: "{task-2}", description: "{details}", assignee: "{teammate-2}" },
     { title: "{task-3}", description: "{details}", depends_on: ["{task-1}"] },
     ...
   ])
   ```

   > 5–6 tasks per member is appropriate. Use `depends_on` to declare dependencies between tasks.

### Phase 3: {Primary Work — e.g., Research / Generation / Analysis}

**Execution mode:** Team members self-coordinate

Team members claim tasks from the shared task list and complete them independently.
The leader monitors progress and intervenes when necessary.

**Inter-member communication rules:**
- {teammate-1} sends {what information} to {teammate-2} via SendMessage
- {teammate-2} saves results to a file upon task completion and notifies the leader
- If a team member needs another member's results, they request them via SendMessage

**Output storage:**

| Member | Output Path |
|--------|------------|
| {teammate-1} | `_workspace/{phase}_{teammate-1}_{artifact}.md` |
| {teammate-2} | `_workspace/{phase}_{teammate-2}_{artifact}.md` |

**Leader monitoring:**
- Automatically notified when a team member becomes idle
- Sends instructions via SendMessage or reassigns tasks when a member is blocked
- Checks overall progress with TaskGet

### Phase 4: {Follow-up Work — e.g., Validation / Integration}
1. Wait for all team members to complete their tasks (check status with TaskGet)
2. Collect each member's output with Read
3. {Integration/validation logic}
4. Produce final output: `{output-path}/{filename}`

### Phase 5: Cleanup
1. Ask team members to shut down (SendMessage)
2. Disband the team (TeamDelete)
3. Preserve the `_workspace/` directory (do not delete intermediate outputs — kept for post-hoc verification and audit trails)
4. Report results summary to the user

> **When team reassembly is needed:** If a different specialist combination is required for different phases, clean up the current team with TeamDelete and then create the next phase's team with a new TeamCreate. The previous team's outputs are preserved in `_workspace/` and are accessible to the new team via Read.

## Data Flow

```
[Leader] → TeamCreate → [teammate-1] ←SendMessage→ [teammate-2]
                            │                           │
                            ↓                           ↓
                      artifact-1.md              artifact-2.md
                            │                           │
                            └───────── Read ────────────┘
                                         ↓
                                  [Leader: Integration]
                                         ↓
                                   Final Output
```

## Error Handling

| Situation | Strategy |
|-----------|----------|
| One member fails/stops | Leader detects → checks status via SendMessage → restarts or creates a replacement member |
| Majority of members fail | Notify the user and confirm whether to proceed |
| Timeout | Use partial results collected so far, shut down incomplete members |
| Data conflict between members | Record both with source attribution; do not delete |
| Delayed task status | Leader checks with TaskGet then manually calls TaskUpdate |

## Test Scenarios

### Happy Path
1. User provides {input}
2. Phase 1 yields {analysis result}
3. Phase 2 assembles the team ({N} members + {M} tasks)
4. Phase 3: team members self-coordinate and complete their work
5. Phase 4: integrate outputs to produce the final result
6. Phase 5: disband the team
7. Expected result: `{output-path}/{filename}` is created

### Error Path
1. {teammate-2} stops with an error during Phase 3
2. Leader receives idle notification
3. Checks status via SendMessage → attempts restart
4. If restart fails, reassign {teammate-2}'s tasks to {teammate-1}
5. Proceed to Phase 4 with the remaining results
6. Note "{teammate-2} section partially uncollected" in the final report
```

---

## Template B: Sub-Agent Mode (Lightweight)

Sub-agents are invoked directly with the `Agent` tool and return results only to the main agent.

```markdown
---
name: {domain}-orchestrator
description: "Orchestrator that coordinates {domain} agents. {trigger keywords}."
---

# {Domain} Orchestrator

An integrated skill that coordinates {domain} agents to produce {final output}.

## Execution Mode: Sub-Agent

## Agent Configuration

| Agent | subagent_type | Role | Skill | Output |
|-------|--------------|------|-------|--------|
| {agent-1} | {custom or built-in type} | {role} | {skill} | {output-file} |
| {agent-2} | {custom or built-in type} | {role} | {skill} | {output-file} |
| ... | | | | |

## Workflow

### Phase 1: Preparation
1. Analyze user input — {what to determine}
2. Create `_workspace/` in the working directory
3. Save input data to `_workspace/00_input/`

### Phase 2: {Primary Work — e.g., Research / Generation / Analysis}

**Execution mode:** {parallel | sequential | conditional}

{If parallel}
Invoke N Agent tools simultaneously within a single message:

| Agent | Input | Output | model | run_in_background |
|-------|-------|--------|-------|-------------------|
| {agent-1} | {input source} | `_workspace/{phase}_{agent}_{artifact}.md` | opus | true |
| {agent-2} | {input source} | `_workspace/{phase}_{agent}_{artifact}.md` | opus | true |

{If sequential}
Pass the output of each agent as input to the next:

1. Run {agent-1} → produces `_workspace/01_{artifact}.md`
2. Run {agent-2} (input: output of step 1) → produces `_workspace/02_{artifact}.md`

### Phase 3: {Follow-up Work — e.g., Validation / Integration}
1. Collect Phase 2 outputs with Read
2. {Integration/validation logic}
3. Produce final output: `{output-path}/{filename}`

### Phase 4: Cleanup
1. Preserve the `_workspace/` directory (do not delete intermediate outputs — kept for post-hoc verification and audit trails)
2. Report results summary to the user

## Data Flow

```
Input → [agent-1] → artifact-1 ─┐
                                  ├→ [Integration] → Final Output
Input → [agent-2] → artifact-2 ─┘
```

## Error Handling

| Situation | Strategy |
|-----------|----------|
| One agent fails | Retry once. If it fails again, proceed without that result and note the gap in the report |
| Majority of agents fail | Notify the user and confirm whether to proceed |
| Timeout | Use partial results collected so far |
| Data conflict between agents | Record both with source attribution; do not delete |

## Test Scenarios

### Happy Path
1. User provides {input}
2. Phase 1 yields {analysis result}
3. Phase 2: {N} agents run in parallel, each producing their output
4. Phase 3: integrate outputs to produce the final report
5. Expected result: `{output-path}/{filename}` is created

### Error Path
1. {agent-2} fails during Phase 2
2. Fails again after one retry
3. Proceed to Phase 3 with the remaining results, without {agent-2}'s output
4. Note "{agent-2} section data not collected" in the final report
5. Notify the user of partial completion
```

---

## Authoring Principles

1. **Declare the execution mode first** — State "Agent Team" or "Sub-Agent" at the top of the orchestrator
2. **Be specific about TeamCreate/SendMessage/TaskCreate usage in Agent Team mode** — team assembly, task registration, and communication rules
3. **Specify all parameters of the Agent tool in Sub-Agent mode** — name, subagent_type, prompt, run_in_background
4. **Use absolute file paths** — no relative paths; clear paths relative to `_workspace/`
5. **Declare inter-phase dependencies** — which phase depends on the results of which other phase
6. **Keep error handling realistic** — do not assume everything will succeed
7. **Test scenarios are mandatory** — at least 1 happy path + 1 error path

## Reference Orchestrators

The basic structure of a fan-out/fan-in pattern orchestrator:
Preparation → TeamCreate + TaskCreate → N members run in parallel → Read + Integration → Cleanup.
See the research team example in `references/team-examples.md`.
