# Skill Writing Guide

A detailed guide for producing high-quality skills in the harness. Supplementary reference for SKILL.md Phase 4.

---

## Table of Contents

1. [Description Writing Patterns](#1-description-writing-patterns)
2. [Body Writing Style](#2-body-writing-style)
3. [Output Format Definition Patterns](#3-output-format-definition-patterns)
4. [Example Writing Patterns](#4-example-writing-patterns)
5. [Progressive Disclosure Pattern](#5-progressive-disclosure-pattern)
6. [Script Bundling Decision Criteria](#6-script-bundling-decision-criteria)
7. [Data Schema Standards](#7-data-schema-standards)
8. [What Not to Include in a Skill](#8-what-not-to-include-in-a-skill)

---

## 1. Description Writing Patterns

The description is the only trigger mechanism for a skill. Claude decides whether to use a skill based solely on the name + description it sees in the `available_skills` list.

### Understanding the Trigger Mechanism

Claude tends not to invoke a skill for simple tasks it can handle easily with its built-in tools. Even with a perfect description, a plain request like "read this PDF for me" may not trigger the skill. The more complex, multi-step, and specialized the task, the higher the likelihood of triggering the skill.

### Writing Principles

1. Describe both **what the skill does** and **the specific situations that should trigger it**
2. Include boundary conditions that distinguish the skill from similar cases that should not trigger it
3. Be slightly "pushy" — compensate for Claude's tendency to err on the conservative side when deciding whether to trigger

### Good Examples

```yaml
description: "Handles all PDF operations: reading, text/table extraction, merging,
  splitting, rotating, watermarking, encrypting/decrypting, and OCR. Use this
  skill whenever a .pdf file is mentioned or a PDF output is requested. Especially
  useful when conversion, editing, or analysis is needed — not just a simple read."
```

```yaml
description: "Handles all spreadsheet tasks for Excel/CSV/TSV files: adding columns,
  formula calculation, formatting, charts, and data cleaning. Use this skill whenever
  the user mentions a spreadsheet file — even casually ('the xlsx in my downloads
  folder')."
```

### Bad Examples

- `"A skill that processes data"` — too vague; the file type and operation are unclear
- `"PDF-related tasks"` — no specific operations listed; trigger situations not described

---

## 2. Body Writing Style

### Why-First Principle

When an LLM understands the reason behind a rule, it makes correct judgments even in edge cases. Conveying context is more effective than issuing hard commands.

**Bad:**
```markdown
ALWAYS use pdfplumber for table extraction. NEVER use PyPDF2 for tables.
```

**Good:**
```markdown
Use pdfplumber for table extraction. PyPDF2 is optimized for text extraction
and does not preserve the row/column structure of tables. pdfplumber recognizes
cell boundaries and returns structured data.
```

### Generalization Principle

When a problem is discovered through feedback or test results, generalize the fix to the underlying principle rather than narrowing it to the specific example.

**Overfitted fix:**
```markdown
If a column named "Q4 Revenue" is present, convert that column to a number.
```

**Generalized fix:**
```markdown
If a column name contains keywords that imply a numeric value — such as "revenue",
"amount", or "quantity" — convert that column to a numeric type. If conversion
fails, retain the original value.
```

### Imperative Tone

Use direct imperative forms ("Do X", "Use Y") rather than hedged constructions ("You can do X", "It is possible to do Y"). A skill is a set of instructions.

### Context Economy

The context window is a shared resource. Ask whether every sentence justifies its token cost:
- "Does Claude already know this?" → Delete it
- "Would Claude make a mistake without this explanation?" → Keep it
- "Is one concrete example more effective than a long explanation?" → Replace with an example

---

## 3. Output Format Definition Patterns

Use this pattern in skills where the format of the output matters:

```markdown
## Report Structure
Follow this template exactly:

# [Title]
## Summary
## Key Findings
## Recommendations
```

Keep format definitions concise. Including a real example makes them even more effective.

---

## 4. Example Writing Patterns

Examples are more effective than long explanations:

```markdown
## Commit Message Format

**Example 1:**
Input: Add JWT token-based user authentication
Output: feat(auth): implement JWT-based authentication

**Example 2:**
Input: Fix bug where the show-password button on the login page does not work
Output: fix(login): repair password visibility toggle button
```

---

## 5. Progressive Disclosure Pattern

### Pattern 1: Domain-Based Separation

```
bigquery-skill/
├── SKILL.md (overview + domain selection guide)
└── references/
    ├── finance.md (revenue, billing metrics)
    ├── sales.md (opportunities, pipeline)
    └── product.md (API usage, features)
```

When the user asks about revenue, load only finance.md.

### Pattern 2: Conditional Detail

```markdown
# DOCX Processing

## Document Creation
Create a new document with docx-js. → See [DOCX-JS.md](references/docx-js.md).

## Document Editing
For simple edits, modify the XML directly.
**If tracked changes are needed**: See [REDLINING.md](references/redlining.md)
```

### Pattern 3: Large Reference File Structure

Reference files over 300 lines should include a table of contents at the top:

```markdown
# API Reference

## Table of Contents
1. [Authentication](#authentication)
2. [Endpoint List](#endpoint-list)
3. [Error Codes](#error-codes)
4. [Rate Limits](#rate-limits)

---

## Authentication
...
```

---

## 6. Script Bundling Decision Criteria

Observe agent transcripts during test runs. The following patterns are candidates for bundling:

| Signal | Action |
|--------|--------|
| The same helper script generated in all 3 out of 3 tests | Bundle into `scripts/` |
| The same `pip install`/`npm install` run every time | Add a dependency installation step to the skill |
| The same multi-step approach repeated each time | Describe it as a standard procedure in the skill body |
| A similar error encountered each time, followed by the same workaround | Document the known issue and fix in the skill |

Bundled scripts must pass an execution test before being included.

---

## 7. Data Schema Standards

Use standard schemas to ensure consistent data exchange between skills. These can also be used for testing and evaluating skills produced by the harness.

### eval_metadata.json

Metadata for each test case:

```json
{
  "eval_id": 0,
  "eval_name": "descriptive-name-here",
  "prompt": "The user's task prompt",
  "assertions": [
    "The output contains X",
    "A file was created in Y format"
  ]
}
```

### grading.json

Assertion-based grading results:

```json
{
  "expectations": [
    {
      "text": "Output contains 'Seoul'",
      "passed": true,
      "evidence": "Confirmed 'Seoul region data extracted' at step 3"
    }
  ],
  "summary": {
    "passed": 2,
    "failed": 1,
    "total": 3,
    "pass_rate": 0.67
  }
}
```

**Field name note:** Use `text`, `passed`, and `evidence` exactly as shown. Do not use variations such as `name`/`met`/`details`.

### timing.json

Execution time and token measurements:

```json
{
  "total_tokens": 84852,
  "duration_ms": 23332,
  "total_duration_seconds": 23.3
}
```

Save `total_tokens` and `duration_ms` immediately from the sub-agent completion notification. This data is only accessible at the time of the notification and cannot be recovered later.

---

## 8. What Not to Include in a Skill

- Supplementary documents such as README.md, CHANGELOG.md, or INSTALLATION_GUIDE.md
- Meta-information about the skill creation process (test results, iteration history)
- User-facing documentation (a skill is a set of instructions for an AI agent, not a human)
- General knowledge that Claude already knows
