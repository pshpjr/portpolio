# Agent Team Design Patterns

## Execution Modes: Agent Teams vs Sub-agents

Understand the core difference between the two execution modes and choose the one that fits.

### Agent Teams — Default Mode

The team leader creates a team with `TeamCreate`, and team members run as independent Claude Code instances. Members communicate directly via `SendMessage` and self-coordinate through a shared task list (`TaskCreate`/`TaskUpdate`).

```
[Leader] ←→ [Member A] ←→ [Member B]
  ↕              ↕              ↕
  └──── Shared Task List ────┘
```

**Core tools:**
- `TeamCreate`: Create the team and spawn members
- `SendMessage({to: name})`: Message a specific member
- `SendMessage({to: "all"})`: Broadcast (high cost — use sparingly)
- `TaskCreate`/`TaskUpdate`: Manage the shared task list

**Characteristics:**
- Members can talk directly, challenge each other, and cross-validate
- Information flows between members without routing through the leader
- Self-coordination via the shared task list (members can request their own tasks)
- Leader is automatically notified when a member becomes idle
- Plan-approval mode allows review before risky operations

**Constraints:**
- Only one team can be **active** per session (though a team can be dissolved between phases and a new one created)
- No nested teams (a member cannot create its own team)
- Leader is fixed (cannot be transferred)
- High token cost

**Team reconstruction pattern:**
When different specialist combinations are needed across phases, save the previous team's outputs to files, dissolve the team, then create a new team. Previous outputs are preserved under `_workspace/`, so the new team can access them via Read.

### Sub-agents — Lightweight Mode

The main agent spawns sub-agents using the `Agent` tool. Sub-agents return results only to the main agent and do not communicate with each other.

```
[Main] → [Sub A] → return result
       → [Sub B] → return result
       → [Sub C] → return result
```

**Core tools:**
- `Agent(prompt, subagent_type, run_in_background)`: Spawn a sub-agent

**Characteristics:**
- Lightweight and fast
- Results are returned as a summary to the main agent's context
- Token-efficient

**Constraints:**
- No communication between sub-agents
- Main agent handles all coordination
- No real-time collaboration or challenge

### Mode Selection Decision Tree

```
Are there 2 or more agents?
├── Yes → Do the agents need to communicate with each other?
│         ├── Yes → Agent Team (default)
│         │         Cross-validation, shared discoveries, and real-time feedback
│         │         drive quality improvement.
│         │
│         └── No → Sub-agents are also viable
│                  Produce-and-review pipelines or expert pools that only
│                  need result handoff.
│
└── No (1 agent) → Sub-agent
                   A single agent needs no team.
```

> **Core principle:** Agent teams are the default. When choosing sub-agents, ask yourself: "Is inter-member communication truly unnecessary?"

---

## Agent Team Architecture Types

### 1. Pipeline
Sequential task flow. The output of one agent becomes the input of the next.

```
[Analyze] → [Design] → [Implement] → [Verify]
```

**When to use:** Each stage depends heavily on the output of the previous one
**Example:** Novel writing — worldbuilding → characters → plot → writing → editing
**Caution:** A bottleneck stalls the entire pipeline. Design each stage to be as independent as possible.
**Team mode fit:** Strong sequential dependencies limit the benefits of team mode. However, team mode is useful if the pipeline contains parallel segments.

### 2. Fan-out/Fan-in
Parallel processing followed by result aggregation. Independent tasks run simultaneously.

```
           ┌→ [Expert A] ─┐
[Dispatch] → ├→ [Expert B] ─┼→ [Integrate]
           └→ [Expert C] ─┘
```

**When to use:** The same input requires analysis from multiple different perspectives or domains
**Example:** Comprehensive research — official sources / media / community / background investigated simultaneously → consolidated report
**Caution:** The quality of the integration stage determines the overall quality.
**Team mode fit:** This is the most natural pattern for agent teams. **Always build this as an agent team.** Members share discoveries and challenge each other, so one agent's finding can redirect another's investigation in real time — yielding significantly higher quality than independent research.

### 3. Expert Pool
Selectively invoke the right expert based on context.

```
[Router] → { Expert A | Expert B | Expert C }
```

**When to use:** Different processing is needed depending on the type of input
**Example:** Code review — invoke only the relevant specialist from security / performance / architecture experts
**Caution:** The accuracy of the router's classification is critical.
**Team mode fit:** Sub-agents are a better fit. Only the needed expert is called, so a standing team is unnecessary.

### 4. Producer-Reviewer
A producer agent and a reviewer agent operate as a pair.

```
[Produce] → [Review] → (if issues found) → [Produce] re-runs
```

**When to use:** Output quality assurance matters and objective verification criteria exist
**Example:** Webtoon — artist produces → reviewer inspects → problem panels are regenerated
**Caution:** Set a maximum retry count (2–3) to prevent infinite loops.
**Team mode fit:** Agent teams are useful here. SendMessage enables real-time feedback between producer and reviewer.

### 5. Supervisor
A central agent manages task state and dynamically dispatches work to worker agents.

```
             ┌→ [Worker A]
[Supervisor] ─┼→ [Worker B]    ← Supervisor monitors state and dispatches dynamically
             └→ [Worker C]
```

**When to use:** Workload is variable or task distribution must be decided at runtime
**Example:** Large-scale code migration — supervisor analyzes the file list and assigns batches to workers
**Difference from Fan-out:** Fan-out assigns tasks up front; supervisor adjusts distribution dynamically as work progresses
**Caution:** Set delegation units large enough to prevent the supervisor from becoming a bottleneck.
**Team mode fit:** The shared task list in agent teams maps naturally to the supervisor pattern. Register tasks with TaskCreate; team members claim them on their own.

### 6. Hierarchical Delegation
Higher-level agents delegate recursively to lower-level agents. Decomposes complex problems step by step.

```
[Director] → [Team Lead A] → [Worker A1]
                            → [Worker A2]
           → [Team Lead B] → [Worker B1]
```

**When to use:** The problem naturally decomposes into a hierarchical structure
**Example:** Full-stack app development — director → frontend lead → (UI / logic / tests) + backend lead → (API / DB / tests)
**Caution:** Three or more levels of depth introduce significant latency and context loss. Keep it to two levels or fewer.
**Team mode fit:** Agent teams cannot be nested (a member cannot create a team). Implement level 1 as a team and level 2 as sub-agents, or flatten the hierarchy into a single team.

## Composite Patterns

Real-world scenarios more commonly use composite patterns than single ones:

| Composite Pattern | Composition | Example |
|------------------|-------------|---------|
| **Fan-out + Producer-Reviewer** | Parallel production, then individual review | Multilingual translation — 4 languages translated in parallel → each reviewed by a native-language reviewer |
| **Pipeline + Fan-out** | Some sequential stages are parallelized | Analyze (sequential) → Implement (parallel) → Integration test (sequential) |
| **Supervisor + Expert Pool** | Supervisor dynamically invokes experts | Customer inquiry handling — supervisor classifies the inquiry then assigns the right expert |

### Execution Mode in Composite Patterns

**By default, use agent teams for all composite patterns.** Active communication between members is the primary driver of output quality.

| Scenario | Recommended Mode | Reason |
|---------|-----------------|--------|
| **Research + Analysis** | Agent team | Investigators share findings; conflicting information is debated in real time |
| **Design + Implementation + Verification** | Agent team | Feedback loops between designer ↔ implementer ↔ verifier |
| **Supervisor + Workers** | Agent team | Dynamic assignment via shared task list; workers share progress with each other |
| **Produce + Review** | Agent team | Real-time feedback between producer ↔ reviewer minimizes rework |

> Consider mixing in sub-agents only when a single agent performs a fully isolated, one-shot task.

## Agent Type Selection

When invoking an agent, specify the type via the `subagent_type` parameter of the Agent tool. Team members can also use custom agent definitions.

### Built-in Types

| Type | Tool Access | Best For |
|------|------------|---------|
| `general-purpose` | Full (includes WebSearch, WebFetch) | Web research, general-purpose tasks |
| `Explore` | Read-only (no Edit/Write) | Codebase exploration, analysis |
| `Plan` | Read-only (no Edit/Write) | Architecture design, planning |

### Custom Types

Define an agent in `.claude/agents/{name}.md` and invoke it with `subagent_type: "{name}"`. Custom agents have full tool access.

### Selection Criteria

| Situation | Recommended | Reason |
|-----------|------------|--------|
| Complex role reused across multiple sessions | **Custom type** (`.claude/agents/`) | Persona and task principles managed as a file |
| Simple research/collection where a prompt is sufficient | **`general-purpose`** + detailed prompt | No agent file needed; instructions go in the prompt |
| Only needs to read code (analysis/review) | **`Explore`** | Prevents accidental file modification |
| Only needs to design/plan | **`Plan`** | Focused on analysis; prevents code changes |
| Implementation work requiring file modification | **Custom type** | Full tool access + specialized instructions |

**Principle:** Every agent must be defined as a `.claude/agents/{name}.md` file. Even when using a built-in type, create an agent definition file to spell out its role, principles, and protocol. A file must exist for reuse across sessions, and the team communication protocol must be stated explicitly to guarantee collaboration quality.

**Model:** All agents use `model: "opus"`. Always include the `model: "opus"` parameter when calling the Agent tool.

## Agent Definition Structure

```markdown
---
name: agent-name
description: "1–2 sentence role description. List trigger keywords."
---

# Agent Name — One-line Role Summary

You are a [role] specialist in [domain].

## Core Responsibilities
1. Responsibility 1
2. Responsibility 2

## Working Principles
- Principle 1
- Principle 2

## Input/Output Protocol
- Input: [what you receive and from where]
- Output: [what you produce and where it goes]
- Format: [file format, structure]

## Team Communication Protocol (Agent Team Mode)
- Receive: [which messages you receive and from whom]
- Send: [which messages you send and to whom]
- Task requests: [what types of tasks you request from the shared task list]

## Error Handling
- [Action on failure]
- [Action on timeout]

## Collaboration
- Relationships with other agents
```

## Agent Separation Criteria

| Criterion | Separate | Merge |
|-----------|---------|-------|
| Expertise | Domains differ | Domains overlap |
| Parallelism | Can run independently | Sequentially dependent |
| Context | Heavy context burden | Lightweight and fast |
| Reusability | Used by other teams as well | Used only by this team |

## Skills vs Agents

| | Skill | Agent |
|--|-------|-------|
| Definition | Procedural knowledge + tool bundle | Expert persona + behavioral principles |
| Location | `.claude/skills/` | `.claude/agents/` |
| Trigger | User request keyword matching | Explicit invocation via Agent tool |
| Size | Small to large (workflows) | Small (role definition) |
| Purpose | "How to do it" | "Who does it" |

A skill is a **procedural guide** that an agent references when performing a task.
An agent is an **expert role definition** that leverages skills.

## Skill ↔ Agent Integration

Three ways an agent can make use of a skill:

| Method | Implementation | When to Use |
|--------|---------------|-------------|
| **Skill tool invocation** | State `Call /skill-name via the Skill tool` in the agent prompt | When the skill is a standalone workflow and can also be called by users |
| **Inline in prompt** | Embed skill content directly in the agent definition | When the skill is short (50 lines or fewer) and exclusive to this agent |
| **Reference load** | Load the skill's `references/` file on demand via Read | When the skill content is large and only conditionally needed |

Recommendation: use the Skill tool for high-reuse skills, inline for exclusive skills, and reference load for large-content skills.
