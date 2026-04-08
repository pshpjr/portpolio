---
name: harness
description: "Meta-skill for configuring a project harness: define specialist agents, generate the skills they use, and wire up the orchestration. Use when: 'set up agent team', 'build skill', 'define agents', 'configure automation harness', 'design agent architecture for this project', 'build a harness', 'design a harness'."
---

# Harness — Agent Team & Skill Architect

A meta-skill for configuring a harness suited to a domain or project, defining each agent's role, and generating the skills those agents will use.

**Core principles:**
1. **Claude decides what to build** — scope, architecture, task decomposition, and task prompts only.
2. **All file creation and modification is delegated** via `codex-delegate`: Codex and OpenCode headless agents handle execution, and Claude compares or selects the right path based on write ownership.
3. Generate agent definitions (`.claude/agents/`) and skills (`.claude/skills/`) through the delegation model above.
4. **Use an agent team as the default execution mode** for multi-agent collaboration.
5. Harness work is repository work, so `exec-plan-gate` runs first. This is intentional.

## Workflow

### Phase 1: Domain Analysis & Task Routing
1. Identify the domain or project from the user's request.
2. Identify the core task types (creation, validation, editing, analysis, etc.).
3. Check for existing agents and skills to avoid conflicts or duplication.
4. Explore the project codebase — understand the tech stack, data models, and key modules.
5. **Detect user proficiency** — read contextual cues in the conversation (terminology used, question depth) to gauge technical level, and adjust communication tone accordingly. Do not use terms like "assertion" or "JSON schema" without explanation when talking to users with limited coding experience.
6. **Classify task complexity** before choosing execution mode:

| Bucket | Signals | Action |
|--------|---------|--------|
| **Lightweight** | single factual question, one-file trivial edit, quick lookup | Implement directly — no agent team needed |
| **Standard** | one feature/bug across a small number of files, clear goal, moderate context | Single agent or small team, plan optional |
| **Deep** | unclear root cause, architecture tradeoffs, large diff review, mixed implement+validate+migrate | Full agent team with plan, read memory/instructions first |

Decision rules:
- Favor `lightweight` when the request is self-contained and reversible.
- Favor `deep` when root cause, architecture, or evidence gathering dominates.
- If the request mixes "explain", "implement", and "review" in one sentence, default to `deep`.
- If the request mentions "today", "latest", or "current", verify externally before acting on stale state.
- Check whether `_workspace/token-log.jsonl` exists; if so, review recent token costs to calibrate team size (high-cost sessions suggest tighter scoping).

### Phase 2: Team Architecture Design

#### 2-1. Choosing an Execution Mode: Agent Team vs Sub-Agent

**The default is an agent team.** When two or more agents need to collaborate, prefer the agent team approach. Team members self-coordinate through direct communication (SendMessage) and a shared task list (TaskCreate); sharing discoveries, debating trade-offs, and filling in gaps all improve output quality.

Use sub-agents when there is only one agent, or when inter-agent communication is unnecessary (results only need to be passed forward).

> For a comparison table and decision tree, see the "Execution Modes" section in `references/agent-design-patterns.md`.

#### 2-2. Choosing an Architecture Pattern

1. Decompose the work into specialist domains.
2. Decide on the agent team structure (see architecture patterns in `references/agent-design-patterns.md`):
   - **Pipeline**: sequentially dependent tasks
   - **Fan-out / Fan-in**: parallel independent tasks
   - **Expert pool**: selective invocation based on context
   - **Generate-and-validate**: generation followed by quality review
   - **Supervisor**: a central agent manages state and dynamically distributes work
   - **Hierarchical delegation**: a parent agent recursively delegates to children

#### 2-3. Agent Separation Criteria

Evaluate along four axes: specialization, parallelism, context isolation, and reusability. For detailed criteria, see the "Agent Separation Criteria" section in `references/agent-design-patterns.md`.

### Phase 3: Agent Definition Generation

**File writing is always delegated.** Claude writes the content spec and task prompt; the actual files are created through `codex-delegate`, which uses Codex and OpenCode headless agents in the appropriate mode. Compare outputs and apply the best result when the workflow is proposal-based.

**Every agent must be defined in a `project/.claude/agents/{name}.md` file.** Placing role descriptions directly in the Agent tool's prompt without a definition file is not allowed. Reasons:
- Agent definitions must exist as files to be reusable across sessions.
- Team communication protocols must be explicit to ensure reliable inter-agent collaboration.
- The core value of a harness is the separation of agents (who) from skills (how).

Even when using built-in types (`general-purpose`, `Explore`, `Plan`), always create an agent definition file. Specify the built-in type via the `subagent_type` parameter on the Agent tool call, and put the role, principles, and protocols into the definition file.

**Model setting:** All agents use `model: "opus"`. Always specify the `model: "opus"` parameter when invoking the Agent tool. Harness quality is directly tied to the reasoning capability of its agents, and opus delivers the highest quality.

**Team restructuring:** Only one team can be active per session, but teams can be disbanded and rebuilt between phases. If a pipeline pattern requires a different specialist composition per phase, save the current team's outputs to files, clean up the team, and form a new one.

Define each agent in `project/.claude/agents/{name}.md`. Required sections: core role, working principles, input/output protocol, error handling, and collaboration. In agent team mode, add a `## Team Communication Protocol` section specifying which agents to send messages to and from, and the scope of work requests.

> For definition templates and full file examples, see "Agent Definition Structure" in `references/agent-design-patterns.md` and `references/team-examples.md`.

**When including a QA agent — required notes:**
- Use `general-purpose` type for QA agents (`Explore` is read-only and cannot run validation scripts).
- The core of QA is **"cross-boundary comparison"**, not just existence checks — read API responses and frontend hooks simultaneously and compare shapes.
- Run QA **incrementally after each module is complete**, not once at the very end.
- Detailed guide: `references/qa-agent-guide.md`.

### Phase 4: Skill Generation

Create the skills each agent will use at `project/.claude/skills/{name}/SKILL.md`. For a detailed writing guide, see `references/skill-writing-guide.md`.

**File writing is always delegated.** Claude drafts the skill content spec (structure, trigger keywords, body outline); actual SKILL.md files are produced through `codex-delegate`, which uses Codex and OpenCode headless agents in the appropriate mode. Compare outputs and apply the best result when the workflow is proposal-based.

#### 4-1. Skill Structure

```
skill-name/
├── SKILL.md (required)
│   ├── YAML frontmatter (name, description required)
│   └── Markdown body
└── Bundled Resources (optional)
    ├── scripts/    - executable code for repetitive or deterministic tasks
    ├── references/ - reference documents loaded on demand
    └── assets/     - files used in output (templates, images, etc.)
```

#### 4-2. Skill Improvement Workflow

When improving an existing skill rather than creating one:
1. Read the full skill file.
2. Identify what failed or felt ambiguous in actual usage.
3. Tighten the description if triggering was weak.
4. Add missing commands or environment assumptions.
5. Add decision rules where the old version left too much ambiguity.
6. Add failure handling if the skill assumed a happy path.
7. Remove stale instructions and dead commands.
8. If `_workspace/skill-usage-log.jsonl` exists, check actual invocation counts — skills with zero invocations likely have a trigger problem.

#### 4-3. Writing the Description — Actively Encourage Triggering

The description is the skill's sole trigger mechanism. Because Claude tends to be conservative about triggering, write descriptions **aggressively ("pushy")**.

**Bad example:** `"A skill for processing PDF documents"`
**Good example:** `"Handles all PDF operations: reading, text/table extraction, merging, splitting, rotating, watermarking, encrypting, OCR, and more. This skill must be used whenever a .pdf file is mentioned or a PDF output is requested."`

The key: describe both what the skill does and the specific trigger situations, and distinguish it clearly from similar cases where this skill should not fire.

#### 4-3. Body Writing Principles

| Principle | Description |
|-----------|-------------|
| **Explain the why** | Instead of imperatives like "ALWAYS/NEVER", convey the reason behind each rule. When an LLM understands the reason, it makes correct decisions even in edge cases. |
| **Keep it lean** | The context window is a shared resource. Aim to keep the SKILL.md body under 500 lines; move anything that adds weight without adding value into `references/`. |
| **Generalize** | Explain underlying principles rather than narrow rules that only fit specific examples. Avoid overfitting. |
| **Bundle repetitive code** | If agents are found to write the same helper scripts across tests, pre-bundle that code in `scripts/`. |
| **Write in the imperative** | Use directive language: "do this", "always do X". |

#### 4-4. Progressive Disclosure

Skills manage context through a three-tier loading system:

| Tier | When loaded | Size target |
|------|-------------|-------------|
| **Metadata** (name + description) | Always in context | ~100 words |
| **SKILL.md body** | When the skill is triggered | <500 lines |
| **references/** | Only when needed | Unlimited (scripts can be executed without loading) |

**Size management rules:**
- When SKILL.md approaches 500 lines, move detailed content to `references/` and leave a pointer in the body indicating when to read that file.
- Reference files over 300 lines must include a **table of contents** at the top.
- If there are domain- or framework-specific variants, split them under `references/` by domain so only the relevant file is loaded.

```
cloud-deploy/
├── SKILL.md (workflow + selection guide)
└── references/
    ├── aws.md    ← load only when AWS is selected
    ├── gcp.md
    └── azure.md
```

#### 4-5. Skill-to-Agent Mapping Principles

- One agent ↔ one or more skills (1:1 or 1:many)
- A skill can be shared by multiple agents
- Skills capture "how to do it"; agents capture "who does it"

> For detailed writing patterns, examples, and data schema standards, see `references/skill-writing-guide.md`.

### Phase 5: Integration and Orchestration

An orchestrator is a special form of skill that ties individual agents and skills into a single workflow and coordinates the entire team. Where the individual skills created in Phase 4 define "what each agent does and how", the orchestrator defines "who collaborates in what order and when". For a concrete template, see `references/orchestrator-template.md`.

The orchestrator pattern differs depending on execution mode:

#### 5-0. Orchestrator Pattern by Mode

**Agent team mode (default):**
The orchestrator forms the team with `TeamCreate` and assigns work with `TaskCreate`. Team members communicate directly via `SendMessage` and self-coordinate. The leader (orchestrator) monitors progress and consolidates results.

```
[Orchestrator / Leader]
    ├── TeamCreate(team_name, members)
    ├── TaskCreate(tasks with dependencies)
    ├── Team members self-coordinate (SendMessage)
    ├── Collect and consolidate results
    └── Clean up team
```

**Sub-agent mode:**
The orchestrator calls sub-agents directly via the `Agent` tool. Sub-agents return results only to the orchestrator.

```
[Orchestrator]
    ├── Agent(agent-1, run_in_background=true)
    ├── Agent(agent-2, run_in_background=true)
    ├── Wait for and collect results
    └── Generate consolidated output
```

#### 5-1. Data Transfer Protocol

Specify data-passing strategies between agents inside the orchestrator:

| Strategy | Method | Execution mode | Best for |
|----------|--------|----------------|----------|
| **Message-based** | Direct communication between team members via `SendMessage` | Agent team | Real-time coordination, feedback exchange, lightweight state transfer |
| **Task-based** | Share work status via `TaskCreate` / `TaskUpdate` | Agent team | Progress tracking, dependency management, explicit work requests |
| **File-based** | Write and read files at agreed-upon paths | Both | Large data, structured outputs, audit trail requirements |

**Recommended combination for agent team mode:** task-based (coordination) + file-based (outputs) + message-based (real-time communication)

Rules for file-based transfer:
- Create a `_workspace/` folder under the working directory for intermediate outputs.
- File naming convention: `{phase}_{agent}_{artifact}.{ext}` (e.g., `01_analyst_requirements.md`)
- Output only final artifacts to user-specified paths; preserve intermediate files (`_workspace/`) for post-run verification and audit trails.

#### 5-2. Error Handling

Include error-handling policy inside the orchestrator. Core principle: retry once; if it fails again, continue without that result (note the gap in the report); do not discard conflicting data — record it with its source.

> For a per-error-type strategy table and implementation details, see the "Error Handling" section in `references/orchestrator-template.md`.

#### 5-3. Team Mode Only: Team Size Guidelines

| Task scale | Recommended team size | Tasks per member |
|------------|-----------------------|------------------|
| Small (5–10 tasks) | 2–3 members | 3–5 tasks |
| Medium (10–20 tasks) | 3–5 members | 4–6 tasks |
| Large (20+ tasks) | 5–7 members | 4–5 tasks |

> More team members means higher coordination overhead. Three focused members outperform five scattered ones.

### Phase 6: Validation and Testing

Validate the generated harness. For detailed testing methodology, see `references/skill-testing-guide.md`.

#### 6-1. Structural Validation

- Verify all agent files are in the correct locations.
- Validate skill frontmatter (name, description).
- Check cross-agent reference consistency.
- Confirm no commands were generated.

#### 6-2. Validation by Execution Mode

- Agent team mode: verify communication paths between team members, task dependencies, and appropriate team size.
- Sub-agent mode: verify input/output connections for each agent and `run_in_background` settings.

#### 6-3. Skill Execution Testing

Run actual execution tests for each generated skill:

1. **Write test prompts** — write 2–3 realistic test prompts per skill. Use concrete, natural sentences that an actual user would type.

2. **Run with-skill vs without-skill comparisons** — where possible, run both in parallel to verify the skill's added value. Spawn two sub-agents for each test:
   - **With-skill**: reads the skill and performs the task
   - **Without-skill (baseline)**: performs the same prompt without the skill

3. **Evaluate results** — assess output quality both qualitatively (user review) and quantitatively (assertion-based). Define assertions when outputs are objectively verifiable (file creation, data extraction, etc.); rely on user feedback for subjective cases (writing style, design).

4. **Iterative improvement loop** — when issues are found in test results:
   - **Generalize** the feedback and revise the skill (avoid narrow fixes that only address a specific example).
   - Retest after revision.
   - Repeat until the user is satisfied or further meaningful improvement is no longer possible.

5. **Bundle recurring patterns** — if agents consistently produce the same code during test runs (e.g., the same helper script in every test), pre-bundle that code into `scripts/`.

#### 6-4. Trigger Validation

Verify that each skill's description triggers correctly:

1. **Should-trigger queries** (8–10) — a variety of phrasings that should trigger the skill (formal/casual, explicit/implicit)
2. **Should-NOT-trigger queries** (8–10) — "near-miss" queries whose keywords are similar but where a different tool or skill is more appropriate

**Key to writing near-miss queries:** Queries like "write a Fibonacci function" are obviously unrelated and have no test value. Good test cases are **queries with ambiguous boundaries**, such as "extract the chart from this Excel file as PNG" (xlsx skill vs image conversion).

Also check for trigger conflicts with existing skills at this stage.

#### 6-5. Skill Audit Checklist

When auditing existing skills (inventory or post-generation review), evaluate each skill on these dimensions:

| Dimension | What to check |
|-----------|--------------|
| Trigger clarity | Does the description contain specific trigger situations, not just "a skill for X"? |
| Instruction quality | Are steps actionable with exact commands, not vague guidance? |
| Command accuracy | Do referenced CLI commands, file paths, and flags still exist? |
| Decision rules | Does the skill tell the agent what to do at ambiguity points? |
| Output contract | Is the expected output format defined? |
| Verification steps | Does the skill say how to confirm success? |
| Duplication | Does it overlap with another skill's trigger or body? If so, merge or differentiate. |
| Staleness | Does the skill reference removed files, renamed APIs, or old conventions? |

Use `_workspace/skill-usage-log.jsonl` (if available) to identify skills with zero invocations — these likely have trigger or relevance problems.

#### 6-5. Dry-Run Test

- Review whether the orchestrator skill's phase order is logically sound.
- Confirm there are no dead links in data transfer paths.
- Verify that every agent's inputs match the outputs from the preceding phase.
- Confirm that fallback paths for each error scenario are executable.

#### 6-6. Writing Test Scenarios

- Add a `## Test Scenarios` section to the orchestrator skill.
- Document at least one normal flow and one error flow.

#### 6-7. Recursive Improvement Loop

If harness design or validation exposes reusable friction, do not leave that insight only in chat.

- Record memorable patterns in the repository feedback log.
- Record independent follow-up work in `docs/workflow/harness-improvement-queue.md`.
- Queue items must include `task` and `message` so another agent can claim and execute them without replaying the full conversation.
- If a script or new skill would likely reduce repeated context or token use, capture that in the queued item as part of the improvement signal.
- For this repository, the default processor for queued harness work is `harness-improver`.

## Output Checklist

Verify after generation is complete:

- [ ] `project/.claude/agents/` — **agent definition files must be created** (required even for built-in types)
- [ ] `project/.claude/skills/` — skill files (SKILL.md + references/)
- [ ] One orchestrator skill (includes data flow + error handling + test scenarios)
- [ ] Execution mode stated explicitly (agent team or sub-agent)
- [ ] `model: "opus"` parameter specified on all Agent tool calls
- [ ] `.claude/commands/` — nothing created here
- [ ] No conflicts with existing agents or skills
- [ ] Skill descriptions written aggressively ("pushy")
- [ ] SKILL.md body is under 500 lines; anything over that split into references/
- [ ] Execution validated with 2–3 test prompts
- [ ] Trigger validation (should-trigger + should-NOT-trigger) complete
- [ ] Recursive improvement path wired (feedback log + task/message queue + owning agent)

## References

- Harness patterns: `references/agent-design-patterns.md`
- Existing harness examples (full file contents): `references/team-examples.md`
- Orchestrator template: `references/orchestrator-template.md`
- **Skill writing guide**: `references/skill-writing-guide.md` — writing patterns, examples, data schema standards
- **Skill testing guide**: `references/skill-testing-guide.md` — testing, evaluation, and iterative improvement methodology
- **QA agent guide**: `references/qa-agent-guide.md` — reference when including a QA agent in a build harness. Covers integration consistency validation methodology, boundary-layer bug patterns, and QA agent definition templates. Based on 7 real bugs found in production projects.
