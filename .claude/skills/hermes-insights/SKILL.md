---
name: hermes-insights
description: Generate insights about your Claude Code usage — what topics you work on most, common patterns, productivity trends.
version: 1.0.0
author: hermes-CCC (ported from Hermes Agent by NousResearch)
license: MIT
metadata:
  hermes:
    tags: [Analytics, Insights, Usage, Productivity]
    related_skills: [hermes-search, honcho]
---

# hermes-insights

Analyze your Claude Code usage patterns across sessions and produce a structured insight report: what topics you work on most, which skills you use, what types of projects dominate your workflow, and where your productivity is strongest.

## Invocation

```
/hermes-insights
/hermes-insights --days <N>
```

---

## Subcommands

### `/hermes-insights` — Full Analysis

Analyzes all available memory files and session history with no time constraint.

### `/hermes-insights --days <N>` — Time-Bounded Analysis

Restricts analysis to session files created in the last N days.

**How Claude filters by date:**

```bash
find ~/.claude/projects/*/memory/session_*.md \
     -mtime -<N> -type f
```

---

## Data Sources and What Claude Analyzes

### Source Priority

This project collects precise telemetry via Claude Code hooks. When JSONL log files exist in `_workspace/`, they are the **primary data source** — they contain exact API-reported numbers, not estimates. Memory files are the fallback for projects without instrumentation.

```bash
# Check which sources are available
ls _workspace/token-log.jsonl _workspace/skill-usage-log.jsonl _workspace/command-log.jsonl 2>/dev/null
```

### 1. Token Usage — `_workspace/token-log.jsonl` (primary)

Each line is a JSON object with exact API usage per session checkpoint:

| Field | Meaning |
|-------|---------|
| `input_tokens` | API-reported input tokens |
| `output_tokens` | API-reported output tokens |
| `cache_creation_input_tokens` | KV cache creation cost |
| `cache_read_input_tokens` | KV cache hits (discounted rate) |
| `total_billed_tokens` | `input_tokens + output_tokens` |
| `turns` | Number of assistant turns in session |
| `models` | Models used in session |
| `session_id` | Session identifier (deduplicate by taking max-turns entry per session) |

**Analysis to produce:**
- Daily/weekly billed token totals and trends
- Average tokens per session and per turn
- Cache hit ratio: `cache_read / (cache_read + cache_creation + input)` — higher is better
- Model distribution across sessions
- Cost efficiency: `total_billed_tokens / changed_lines` (cross-reference with `git log --numstat`)
- Use `python tools/show_token_usage.py --days N` for quick pre-formatted reports

**Fallback:** If this file does not exist, skip token analysis and note "no token instrumentation found".

### 2. Skill & Agent Utilization — `_workspace/skill-usage-log.jsonl` (primary)

Each line records an exact Skill or Agent tool invocation:

| Field | Meaning |
|-------|---------|
| `tool` | `"Skill"` or `"Agent"` |
| `name` | Skill name or subagent_type |
| `args` / `description` | Invocation context (truncated to 120 chars) |

**Analysis to produce:**
- Exact invocation counts per skill/agent, ranked by frequency
- Last-used date per skill
- Skills with zero invocations (trigger problem candidates)
- Agent type distribution (which specialist agents are most used)
- Skill usage trend over time (daily/weekly)

**Fallback:** If this file does not exist, infer skill usage from memory files (mark as `inferred`).

### 3. Command Patterns — `_workspace/command-log.jsonl` (primary)

Each line records a Bash tool call:

| Field | Meaning |
|-------|---------|
| `command` | Executed command (max 300 chars) |
| `purpose` | Agent-written description of intent |
| `script` | Extracted script path (`.py`, `.sh`, etc.) |

**Analysis to produce:**
- Most-used scripts/tools ranked by frequency
- Purpose categories (build, test, deploy, query, etc.)
- Commands without purpose descriptions (quality signal for agent prompting)
- **Repeated command sequences** — same command appearing 3+ times in a session indicates a retry loop or missing automation
- **Failed commands** — if `exit_code` is non-zero, count failure rate per script/command

**Fallback:** If this file does not exist, skip command analysis.

### 4. Waste & Repetition Detection (cross-source)

This is the core analysis for the recursive improvement loop. Cross-reference all three JSONL logs to find token waste candidates:

| Signal | How to detect | Improvement action |
|--------|--------------|-------------------|
| **Retry loops** | Same command ≥3 times in one session in command-log | Diagnose root cause; add error handling to skill or create a script |
| **High-turn sessions** | Sessions with turns ≫ median in token-log | Check if scope was too broad or agent got stuck; tighten exec-plan scoping |
| **Repeated skill+fail** | Same skill invoked multiple times with similar args in skill-usage-log | Fix skill instructions or add precondition checks |
| **Low cache ratio sessions** | `cache_read / (cache_read + cache_creation + input) < 0.8` | Investigate context instability — frequent file switches or large tool outputs |
| **Outlier sessions** | Billed tokens > 2× median | Identify what drove the cost — large file reads, excessive tool calls, or retry loops |
| **Manual work patterns** | Same purpose description appearing across multiple sessions | Candidate for new skill or script automation |

**Output:** A "Waste Candidates" table listing the top 3–5 patterns with estimated token savings if automated/fixed, linked to harness-improvement queue actions.

### 5. Memory Files — Topic and Theme Extraction (fallback)

```bash
ls ~/.claude/projects/*/memory/session_*.md
```

For each session file, Claude reads the `decisions`, `facts_learned`, `artifacts_created`, and `open_issues` sections and extracts noun phrases as topic signals.

**Output:** Top 10 topics, each with session count and a representative example decision or fact.

### 5. Data Hygiene and Confidence Rules

- **Deduplication:** For token-log, keep only the entry with the highest `turns` per `session_id` (later checkpoints are cumulative).
- Normalize topic strings by lowercasing, trimming punctuation, and folding obvious singular/plural variants before counting.
- Prefer JSONL data over memory-file inference whenever both are available.
- Downgrade trend claims to `low confidence` when fewer than 5 sessions match the selected date range.
- Report `no data matched the filter` instead of fabricating empty charts when `--days <N>` returns zero entries.
- Separate unresolved carry-over work from newly opened issues so the resolution rate is not overstated.

---

## Full Output Structure

```
## Claude Code Usage Insights
Generated: 2026-04-08 | Sessions analyzed: 12 | Date range: 2026-04-05 – 2026-04-08

### Token Usage (from _workspace/token-log.jsonl)
| Date       | Sessions | Turns | Billed     | Cache Read | Cache Ratio |
|------------|----------|-------|------------|------------|-------------|
| 2026-04-08 |        3 |    45 |     52.1k  |     8.2M   |       94%   |
| 2026-04-07 |        5 |   120 |    118.3k  |    22.1M   |       91%   |
| 2026-04-05 |        4 |   219 |    109.6k  |    13.1M   |       89%   |

Totals: 280.0k billed | 43.4M cache read | avg 23.3k/session | avg 1.2k/turn

### Skill & Agent Utilization (from _workspace/skill-usage-log.jsonl)
| Name              | Type   | Invocations | Last Used  |
|-------------------|--------|-------------|------------|
| opencode-review   | Skill  |           2 | 2026-04-08 |
| codex-delegate    | Skill  |           1 | 2026-04-08 |
| opencode-reviewer | Agent  |           2 | 2026-04-08 |

Zero-invocation skills (potential trigger issues): hermes-persona, hermes-traj

### Command Patterns (from _workspace/command-log.jsonl)
| Script/Command         | Count | Top Purpose                    |
|------------------------|-------|--------------------------------|
| tools/hi_done.py       |     2 | Mark queue items as done       |
| git status             |     1 | Show working tree status       |
| git log                |     1 | Show recent commits            |

### Efficiency Metrics
- Billed tokens per changed line: 142 tokens/line (lower is better)
- Cache hit ratio trend: 89% → 91% → 94% (improving)
- Median session tokens: 18.2k | Outlier threshold (2×): 36.4k

### Waste Candidates (recursive improvement targets)
| Pattern | Evidence | Est. Savings | Action |
|---------|----------|-------------|--------|
| `git status` retry loop | 4× in session abc123 | ~2k tokens/session | Add pre-check to commit skill |
| codex process timeout | `taskkill /F /IM codex.exe` in 3 sessions | ~5k tokens/session | Add timeout guard to codex-delegate |
| Outlier session 7e58b64a | 47k billed (2.5× median) | — | Scope was too broad; split exec-plan |

→ Register top candidates in `docs/workflow/harness-improvement-queue.md`

### Recommendations
- **Cache efficiency improving** — recent sessions show 94% cache hit ratio, keep using stable context patterns.
- **Zero-invocation skills** — hermes-persona, hermes-traj have never triggered. Review trigger descriptions or remove.
- **Retry loops are the #1 waste source** — automate or add error handling to eliminate re-runs.

### Key Takeaway
12 sessions consumed 280k billed tokens with 92% average cache hit ratio. Top waste candidate: retry loops (~2–5k tokens/session recoverable).
```

---

## How Claude Gathers the Data

1. Check for instrumented JSONL logs in `_workspace/`:
   - `token-log.jsonl` → token usage analysis (deduplicate by max turns per session_id)
   - `skill-usage-log.jsonl` → exact skill/agent invocation counts
   - `command-log.jsonl` → command pattern analysis
2. If `--days N` is specified, filter JSONL entries by `date` field.
3. For token efficiency, cross-reference with `git log --numstat --since=<date>` for changed-line counts.
4. If JSONL logs are absent, fall back to memory files: `ls -t ~/.claude/projects/*/memory/session_*.md`
5. For quick token reports, run `python tools/show_token_usage.py --days N` directly.
6. Aggregate and format the insight report.

Claude does not write any files during `/hermes-insights`. The command is read-only.

---

## Relationship to Hermes `/insights` Command

This skill mirrors the `/insights` command in Hermes Agent (NousResearch), which produces usage analytics from the SQLite session store. In hermes-CCC, the equivalent data source is the Markdown session memory files written by `hermes-compress`. The output categories (topics, skill utilization, productivity trends, recommendations) match the Hermes insights contract.

---

## Notes

- When `_workspace/token-log.jsonl` exists, token analysis is precise (API-reported). No hermes-compress runs needed for quantitative insights.
- Qualitative analysis (topics, decisions) still benefits from hermes-compress session files.
- `/hermes-insights` works best after at least 5 sessions of data accumulation (from either JSONL or memory files).
- Use insights to feed the recursive improvement loop: identify high-cost patterns → register improvement in harness-improvement queue.
