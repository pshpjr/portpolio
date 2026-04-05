# Skill Testing & Iterative Improvement Guide

A methodology for validating and iteratively improving the quality of skills produced by the harness. Supplementary reference for SKILL.md Phase 6.

---

## Table of Contents

1. [Testing Framework Overview](#1-testing-framework-overview)
2. [Writing Test Prompts](#2-writing-test-prompts)
3. [Execution Testing: With-skill vs Baseline](#3-execution-testing-with-skill-vs-baseline)
4. [Quantitative Evaluation: Assertion-Based Scoring](#4-quantitative-evaluation-assertion-based-scoring)
5. [Using Specialist Agents](#5-using-specialist-agents)
6. [Iterative Improvement Loop](#6-iterative-improvement-loop)
7. [Description Trigger Verification](#7-description-trigger-verification)
8. [Workspace Structure](#8-workspace-structure)

---

## 1. Testing Framework Overview

Skill quality validation combines **qualitative evaluation** with **quantitative evaluation**.

| Evaluation Type | Method | Best For |
|----------------|--------|----------|
| **Qualitative** | User directly reviews outputs | Writing style, design, creative work, and other subjective qualities |
| **Quantitative** | Assertion-based automated scoring | File generation, data extraction, code generation, and other objectively verifiable outputs |

Core loop: **Write → Run Tests → Evaluate → Improve → Retest**

---

## 2. Writing Test Prompts

### Principles

Test prompts must be **concrete and natural sentences that real users would actually type**. Abstract or artificial prompts have low test value.

### Bad Examples

```
"Process the PDF"
"Extract the data"
"Generate a chart"
```

### Good Examples

```
"In 'Q4_Sales_Final_v2.xlsx' in my Downloads folder, use column C (revenue) and
column D (cost) to add a profit margin (%) column. Then sort in descending order
by profit margin."
```

```
"Extract the table on page 3 of this PDF and convert it to CSV. The table header
spans two rows — the first row is the category and the second row is the actual
column name."
```

### Prompt Diversity

- Mix **formal / casual** tone
- Mix **explicit / implicit** intent (cases where the file format is stated directly vs. cases where it must be inferred from context)
- Mix **simple / complex** tasks
- Include some prompts with abbreviations, typos, or casual phrasing

### Coverage

Start with 2–3 prompts, designed to cover:
- 1 core use case
- 1 edge case
- (Optional) 1 compound task

---

## 3. Execution Testing: With-skill vs Baseline

### 3-1. Comparison Execution Structure

For each test prompt, spawn two sub-agents **simultaneously**:

**With-skill run:**
```
Prompt: "{test prompt}"
Skill path: {skill path}
Output path: _workspace/iteration-N/eval-{id}/with_skill/outputs/
```

**Baseline run:**
```
Prompt: "{test prompt}"  (identical)
Skill: none
Output path: _workspace/iteration-N/eval-{id}/without_skill/outputs/
```

### 3-2. Choosing a Baseline

| Situation | Baseline |
|-----------|----------|
| Creating a new skill | Run the same prompt without the skill |
| Improving an existing skill | The pre-modification skill version (preserve a snapshot) |

### 3-3. Capturing Timing Data

Save `total_tokens` and `duration_ms` **immediately** from the sub-agent completion notification. This data is only accessible at the time of notification and cannot be recovered afterward.

```json
{
  "total_tokens": 84852,
  "duration_ms": 23332,
  "total_duration_seconds": 23.3
}
```

---

## 4. Quantitative Evaluation: Assertion-Based Scoring

### 4-1. Writing Assertions

When outputs can be verified objectively, define assertions for automated scoring.

**Good assertions:**
- Can be judged true or false objectively
- Have descriptive names so it is clear what is being checked from the result alone
- Validate the core value of the skill

**Bad assertions:**
- Things that always pass regardless of whether the skill is present (e.g., "output exists")
- Things that require subjective judgment (e.g., "is well-written")

### 4-2. Programmatic Verification

When an assertion can be verified with code, write it as a script. This is faster and more reliable than manual inspection and can be reused across iterations.

### 4-3. Beware of Non-Discriminating Assertions

An assertion that passes at 100% in both configurations does not measure the skill's differential value. When you find such an assertion, remove it or replace it with a more challenging one.

### 4-4. Scoring Result Schema

```json
{
  "expectations": [
    {
      "text": "Profit margin column was added",
      "passed": true,
      "evidence": "Confirmed 'profit_margin_pct' column in column E"
    },
    {
      "text": "Sorted in descending order by profit margin",
      "passed": false,
      "evidence": "Original order retained with no sorting applied"
    }
  ],
  "summary": {
    "passed": 1,
    "failed": 1,
    "total": 2,
    "pass_rate": 0.50
  }
}
```

---

## 5. Using Specialist Agents

Using agents in specialist roles during the test and evaluation process improves quality.

### 5-1. Grader

Performs assertion-based scoring, extracts verifiable claims from outputs, and cross-validates them.

**Responsibilities:**
- Pass/fail judgment per assertion with supporting evidence
- Extract factual claims from outputs and verify them
- Provide feedback on the quality of the eval itself (suggest improvements when assertions are too easy or ambiguous)

### 5-2. Comparator (Blind Reviewer)

Anonymizes two outputs as A and B, then judges quality without knowing which was produced with the skill.

**When to use:** When you need to rigorously confirm "is the new version genuinely better?" Can be skipped in routine iterative improvement.

**Judgment criteria:**
- Content: accuracy, completeness
- Structure: organization, formatting, usability
- Overall score

### 5-3. Analyzer

Analyzes statistical patterns in benchmark data:
- Non-discriminating assertions (both configurations pass — no discriminative power)
- High-variance evals (results vary widely across runs — unstable)
- Time/token trade-offs (the skill improves quality but also increases cost)

---

## 6. Iterative Improvement Loop

### 6-1. Collecting Feedback

Show outputs to the user and collect feedback. Empty feedback is interpreted as "no issues."

### 6-2. Improvement Principles

1. **Generalize the feedback** — Narrow fixes that only address the test example are overfitting. Fix at the level of principles.
2. **Remove what earns no weight** — Read the transcript; if the skill is making the agent do unproductive work, delete that part.
3. **Explain the why** — Even if the user's feedback is brief, understand why it matters and reflect that understanding in the skill.
4. **Bundle repetitive work** — If the same helper script is generated in every test run, include it in `scripts/` upfront.

### 6-3. Iteration Procedure

```
1. Revise the skill
2. Re-run all test cases in a new iteration-N+1/ directory
3. Present results to the user (compared against the previous iteration)
4. Collect feedback
5. Revise again → repeat
```

**Exit conditions:**
- The user is satisfied
- All feedback is empty (no issues with any output)
- No further meaningful improvements can be made

### 6-4. Draft → Review Pattern

When revising a skill, write a draft first, then **read it again with fresh eyes** and improve it. Do not try to write it perfectly in one pass; go through a draft-review cycle.

---

## 7. Description Trigger Verification

### 7-1. Writing Trigger Eval Queries

Write 20 eval queries — 10 should-trigger + 10 should-NOT-trigger.

**Query quality criteria:**
- Concrete, natural sentences that real users would actually type
- Include specific details such as file paths, personal context, column names, and company names
- Varied in length, tone, and format
- Focus on **edge cases** rather than clear-cut correct answers

**Should-trigger queries (8–10):**
- The same intent expressed in different ways (formal/casual)
- Cases where the skill or file type is not stated explicitly but is clearly needed
- Niche use cases
- Cases where the query competes with another skill but this skill should win

**Should-NOT-trigger queries (8–10):**
- **Near-misses are the key** — queries with similar keywords but where a different tool or skill is more appropriate
- Queries that are obviously unrelated ("write a Fibonacci function") have no test value
- Adjacent domains, ambiguous phrasing, keyword overlap but different context

### 7-2. Verifying Conflicts with Existing Skills

Confirm that the new skill's description does not overlap with the trigger territory of existing skills:

1. Collect the descriptions from the existing skill list
2. Check that the new skill's should-trigger queries do not mistakenly trigger existing skills
3. If a conflict is found, describe the boundary conditions in the description more precisely

### 7-3. Automated Optimization (Optional Advanced Feature)

When description optimization is needed:

1. Split the 20 eval queries into Train (60%) / Test (40%)
2. Measure trigger accuracy with the current description
3. Analyze failure cases to generate an improved description
4. Select the best description based on the Test set (not the Train set — to prevent overfitting)
5. Repeat up to 5 times

> This process is performed with an automation script using `claude -p`. Token costs are high, so run it as a final step after the skill has stabilized sufficiently.

---

## 8. Workspace Structure

A directory structure for systematically managing test and evaluation results:

```
{skill-name}-workspace/
├── iteration-1/
│   ├── eval-descriptive-name-1/
│   │   ├── eval_metadata.json
│   │   ├── with_skill/
│   │   │   ├── outputs/
│   │   │   ├── timing.json
│   │   │   └── grading.json
│   │   └── without_skill/
│   │       ├── outputs/
│   │       ├── timing.json
│   │       └── grading.json
│   ├── eval-descriptive-name-2/
│   │   └── ...
│   └── benchmark.json
├── iteration-2/
│   └── ...
└── evals/
    └── evals.json
```

**Rules:**
- Eval directories use **descriptive names**, not numbers (e.g., `eval-multi-page-table-extraction`)
- Each iteration is preserved in an independent directory (do not overwrite previous iterations)
- `_workspace/` is never deleted — it is kept for post-hoc verification and audit trails
