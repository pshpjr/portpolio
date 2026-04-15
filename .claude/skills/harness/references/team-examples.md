# Agent Team Examples

## Table of Contents

1. [Example 1: Research Team — Fan-out/Fan-in](#example-1-research-team-agent-team-mode)
2. [Example 2: SF Novel Writing Team — Pipeline + Fan-out](#example-2-sf-novel-writing-team-agent-team-mode)
3. [Example 3: Webtoon Production Team — Generate-Validate](#example-3-webtoon-production-team-sub-agent-mode)
4. [Example 4: Code Review Team — Fan-out + Discussion](#example-4-code-review-team-agent-team-mode)
5. [Example 5: Code Migration Team — Supervisor](#example-5-supervisor-pattern--code-migration-team-agent-team-mode)

---

## Example 1: Research Team (Agent Team Mode)

### Team Architecture: Fan-out/Fan-in
### Execution Mode: Agent Team

```
[Leader/Orchestrator]
    ├── TeamCreate(research-team)
    ├── TaskCreate(4 research tasks)
    ├── Members self-coordinate (SendMessage)
    ├── Collect results (Read)
    └── Produce consolidated report
```

### Agent Composition

| Member | Agent Type | Role | Output |
|--------|-----------|------|--------|
| official-researcher | general-purpose | Official docs/blogs | research_official.md |
| media-researcher | general-purpose | Media/investment | research_media.md |
| community-researcher | general-purpose | Community/social media | research_community.md |
| background-researcher | general-purpose | Background/competition/academic | research_background.md |
| (leader = orchestrator) | — | Consolidated report | consolidated-report.md |

> Research agents use the `general-purpose` built-in type but must be defined as `.claude/agents/{name}.md` files. Each file should specify the agent's role, research scope, and team communication protocol to ensure reusability and collaboration quality.

### Orchestrator Workflow (Agent Team)

```
Phase 1: Preparation
  - Analyze user input (identify topic and research mode)
  - Create _workspace/

Phase 2: Team Assembly
  - TeamCreate(team_name: "research-team", members: [
      { name: "official", prompt: "Investigate official channels..." },
      { name: "media", prompt: "Investigate media/investment trends..." },
      { name: "community", prompt: "Investigate community reactions..." },
      { name: "background", prompt: "Investigate background/competitive landscape..." }
    ])
  - TaskCreate(tasks: [
      { title: "Official channel research", assignee: "official" },
      { title: "Media trend research", assignee: "media" },
      { title: "Community reaction research", assignee: "community" },
      { title: "Background landscape research", assignee: "background" }
    ])

Phase 3: Research Execution
  - 4 members conduct research independently
  - Interesting findings are shared between members via SendMessage
    (e.g., media forwards investment news it found to background)
  - Conflicting information is discussed directly between members
  - Each member saves output file and notifies leader upon completion

Phase 4: Consolidation
  - Leader reads all 4 outputs
  - Generates consolidated report
  - Conflicting information is cited with sources

Phase 5: Cleanup
  - Request member shutdown
  - Disband team
  - Preserve _workspace/ (for post-hoc verification and audit trail)
```

### Team Communication Patterns

```
official ──SendMessage──→ background  (share relevant official announcements)
media ────SendMessage──→ background  (share investment/acquisition information)
community ─SendMessage──→ media      (media-relevant items found in community reactions)
all members ──TaskUpdate──→ shared task list  (progress updates)
leader ←───── idle notification ──── completed member   (automatic)
```

---

## Example 2: SF Novel Writing Team (Agent Team Mode)

### Team Architecture: Pipeline + Fan-out
### Execution Mode: Agent Team

```
Phase 1 (parallel — agent team): worldbuilder + character-designer + plot-architect
  → Coordinate consistency with each other via SendMessage
Phase 2 (sequential): prose-stylist (writing)
Phase 3 (parallel — agent team): science-consultant + continuity-manager (review)
  → Share findings with each other via SendMessage
Phase 4 (sequential): prose-stylist (revise based on review)
```

### Agent Composition

| Member | Agent Type | Role | Skills |
|--------|-----------|------|--------|
| worldbuilder | custom | World-building | world-setting |
| character-designer | custom | Character design | character-profile |
| plot-architect | custom | Plot structure | outline |
| prose-stylist | custom | Style editing + writing | write-scene, review-chapter |
| science-consultant | custom | Science validation | science-check |
| continuity-manager | custom | Consistency validation | consistency-check |

> For agent file structure, see the "Agent Definition Structure" section in `references/agent-design-patterns.md`.

### Team Workflow in Detail

```
Phase 1: TeamCreate(team_name: "novel-team", members: [worldbuilder, character-designer, plot-architect])
         TaskCreate([world-building, character design, plot structure])
         → Members self-coordinate and work in parallel
         → worldbuilder sends SendMessage to character-designer upon completing social structure
         → character-designer sends SendMessage to plot-architect upon completing protagonist profile

Phase 2: Disband Phase 1 team → call prose-stylist as a sub-agent (no team needed for solo writing)
         prose-stylist reads the 3 outputs from _workspace/ and writes
         → saves result to _workspace/02_prose_draft.md

Phase 3: Create new team — TeamCreate(team_name: "review-team", members: [science-consultant, continuity-manager])
         (only one active team per session, but Phase 1 team was disbanded so a new team can be created)
         → Two reviewers examine the draft and share findings with each other
         → science-consultant notifies continuity-manager when a physics error is found
         → Disband team after review is complete

Phase 4: Call prose-stylist as a sub-agent to apply review feedback and produce the final revision
```

---

## Example 3: Webtoon Production Team (Sub-agent Mode)

### Team Architecture: Generate-Validate
### Execution Mode: Sub-agent

> In a generate-validate pattern with only two agents where result hand-off matters more than ongoing communication, sub-agents are the right fit.

```
Phase 1: Agent(webtoon-artist) → generate panels
Phase 2: Agent(webtoon-reviewer) → quality inspection
Phase 3: Agent(webtoon-artist) → regenerate problem panels (up to 2 retries)
```

### Agent Composition

| Agent | subagent_type | Role | Skills |
|-------|--------------|------|--------|
| webtoon-artist | custom | Panel image generation | generate-webtoon |
| webtoon-reviewer | custom | Quality inspection | review-webtoon, fix-webtoon-panel |

> For agent file structure, see the "Agent Definition Structure" section in `references/agent-design-patterns.md`.

### Error Handling

```
Retry policy:
- Panel judged REDO → request regeneration from artist (with specific revision instructions)
- Force PASS after a maximum of 2 retry loops
- If 50% or more of all panels are REDO, suggest prompt revision to the user
```

---

## Example 4: Code Review Team (Agent Team Mode)

### Team Architecture: Fan-out/Fan-in + Discussion
### Execution Mode: Agent Team

> Code review is a prime example where agent teams shine. Reviewers from different perspectives share findings and challenge each other, enabling deeper reviews.

```
[Leader] → TeamCreate(review-team)
    ├── security-reviewer: check for security vulnerabilities
    ├── performance-reviewer: analyze performance impact
    └── test-reviewer: verify test coverage
    → Reviewers share findings with each other (SendMessage)
    → Leader consolidates results
```

### Team Communication Patterns

```
security ──SendMessage──→ performance  ("This SQL query is injectable — please also check from a performance angle")
performance ──SendMessage──→ test      ("Found N+1 query — please check whether related tests exist")
test ────SendMessage──→ security      ("No tests for auth module — what's your priority from a security perspective?")
```

Key point: Reviewers communicate **directly without going through the leader**, enabling fast detection of cross-cutting issues.

---

## Example 5: Supervisor Pattern — Code Migration Team (Agent Team Mode)

### Team Architecture: Supervisor
### Execution Mode: Agent Team

```
[supervisor/leader] → analyze file list → assign batches
    ├→ [migrator-1] (batch A)
    ├→ [migrator-2] (batch B)
    └→ [migrator-3] (batch C)
    ← receive TaskUpdate → assign additional batches or reassign
```

### Agent Composition

| Member | Role |
|--------|------|
| (leader = migration-supervisor) | File analysis, batch distribution, progress management |
| migrator-1~3 | Migrate assigned file batches |

### Supervisor's Dynamic Distribution Logic (using Agent Team)

```
1. Collect full list of target files
2. Estimate complexity (file size, import count, dependencies)
3. Register file batches as tasks via TaskCreate (including dependencies)
4. Members claim tasks on their own
5. When a member reports completion via TaskUpdate:
   - Success → member automatically claims next task
   - Failure → leader confirms cause via SendMessage → reassign or hand off to another member
6. All tasks complete → leader runs integration tests
```

Difference from fan-out: tasks are not fixed up front — they are **dynamically assigned at runtime**. The self-claim feature of the shared task list maps naturally to the supervisor pattern.
