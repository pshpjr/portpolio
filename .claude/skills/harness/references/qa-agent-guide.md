# QA Agent Design Guide

A reference guide for including a QA agent in the build harness. Based on bug patterns discovered in a real project (SatangSlide) and root-cause analysis, it provides a verification methodology for systematically catching defects that QA tends to miss.

---

## Table of Contents

1. Patterns of Defects That QA Agents Miss
2. Integration Coherence Verification
3. QA Agent Design Principles
4. Verification Checklist Template
5. QA Agent Definition Template

---

## 1. Patterns of Defects That QA Agents Miss

### 1-1. Boundary Mismatch

The most frequent type of defect. Two components are each implemented "correctly," but the contract breaks at the point where they connect.

| Boundary | Mismatch Example | Why It Gets Missed |
|----------|-----------------|-------------------|
| API response → frontend hook | API returns `{ projects: [...] }`, hook expects `SlideProject[]` | Each validates fine in isolation; no cross-comparison is done |
| API response field name → type definition | API uses `thumbnailUrl` (camelCase), type uses `thumbnail_url` (snake_case) | TypeScript generics cast it silently; the compiler doesn't catch it |
| File path → link href | Page exists at `/dashboard/create` but link points to `/create` | File structure and href are never cross-checked |
| State transition map → actual status updates | Map defines `generating_template → template_approved`, but the transition is missing in code | Only the existence of the map is confirmed; all update code paths are not traced |
| API endpoint → frontend hook | API exists but no corresponding hook (never called) | API list and hook list are never mapped 1:1 |
| Immediate response → async result | API immediately returns `{ status }`, frontend accesses `data.failedIndices` | Type-only check without distinguishing sync vs async responses |

### 1-2. Why Static Code Review Misses These

- **Limits of TypeScript generics**: `fetchJson<SlideProject[]>()` — compiles fine even when the runtime response is `{ projects: [...] }`
- **`npm run build` passing ≠ correct behavior**: Type casting, `any`, and generics allow builds to succeed while runtime failures occur
- **Existence verification vs. connection verification**: "Does the API exist?" and "Does the API's response match what the caller expects?" are entirely different things to verify

---

## 2. Integration Coherence Verification

**Cross-comparison verification** areas that must be included in every QA agent.

### 2-1. API Response ↔ Frontend Hook Type Cross-Verification

**Method**: Compare the `NextResponse.json()` call site in each API route against the `fetchJson<T>` type parameter in the corresponding hook.

```
Verification steps:
1. Extract the shape of the object passed to NextResponse.json() in the API route
2. Check the T type in fetchJson<T> in the corresponding hook
3. Compare whether the shape and T match
4. Verify wrapping behavior (if the API returns { data: [...] }, does the hook unwrap .data?)
```

**Patterns to watch especially:**
- Pagination APIs: `{ items: [], total, page }` vs. frontend expecting an array
- Mismatches across snake_case DB fields → camelCase API responses → frontend type definitions
- Shape differences between an immediate response (202 Accepted) and the final result

### 2-2. File Path ↔ Link/Router Path Mapping

**Method**: Extract URL paths from page files under `src/app/`, then compare against all `href`, `router.push()`, and `redirect()` values in the code.

```
Verification steps:
1. Extract URL patterns from page.tsx file paths under src/app/
   - (group) → removed from the URL
   - [param] → dynamic segment
2. Collect all href=, router.push(, redirect( values in the code
3. Verify that each link matches an actually existing page path
4. Watch for URL prefix on pages inside route groups (e.g., under dashboard/)
```

### 2-3. State Transition Completeness Tracking

**Method**: Extract all `status:` updates from the code and compare against the state transition map.

```
Verification steps:
1. Extract the list of permitted transitions from the state transition map (STATE_TRANSITIONS)
2. Search all API routes for the .update({ status: "..." }) pattern
3. Verify that each transition is defined in the map
4. Identify transitions defined in the map but never executed in code (dead transitions)
5. Specifically: check that transitions from intermediate states (e.g., generating_template)
   to final states (e.g., template_approved) are not missing
```

### 2-4. API Endpoint ↔ Frontend Hook 1:1 Mapping

**Method**: List all API routes and frontend hooks, then verify they are properly paired.

```
Verification steps:
1. Extract the list of HTTP-method endpoints from route.ts files under src/app/api/
2. Extract the list of fetch call URLs from use*.ts files under src/hooks/
3. Identify API endpoints that no hook calls → flag as "unused"
4. Determine whether "unused" is intentional (e.g., admin API) or a missing call
```

---

## 3. QA Agent Design Principles

### 3-1. Use general-purpose Type, Not explore Type

A QA agent set to `Explore` type can only read. But effective QA requires:
- Grep to search for patterns (extract all `NextResponse.json()` calls)
- Script execution for automated comparison (API shape vs. hook types)
- The ability to make fixes when necessary

**Recommendation**: Set the type to `general-purpose` and explicitly specify the "verify → report → request fix" protocol in the agent definition.

### 3-2. Prioritize "Cross-Comparison" Over "Existence Checks" in Your Checklist

| Weak Checklist | Strong Checklist |
|---------------|-----------------|
| Does the API endpoint exist? | Does the API endpoint's response shape match the corresponding hook's type? |
| Is the state transition map defined? | Does every status update in the code match a transition in the map? |
| Does the page file exist? | Do all links in the code point to pages that actually exist? |
| Is TypeScript strict mode enabled? | Is there any type safety bypassed through generic casting? |

### 3-3. The "Read Both Sides at Once" Principle

To catch boundary bugs, QA cannot read only one side. It must:
- Read the API route **and** the corresponding hook **together**
- Read the state transition map **and** the actual update code **together**
- Read the file structure **and** the link paths **together**

State this principle explicitly in the agent definition.

### 3-4. Run QA Immediately After Each Module Completes, Not After the Full Build

If the orchestrator places QA only at "Phase 4: after full completion":
- Bugs accumulate and the cost of fixing them grows
- Early boundary mismatches propagate into subsequent modules

**Recommended pattern**: Run a cross-verification of each backend API and its corresponding hook immediately upon completion (incremental QA).

---

## 4. Verification Checklist Template

An integration coherence checklist for web applications to include in the QA agent definition.

```markdown
### Integration Coherence Verification (Web App)

#### API ↔ Frontend Connections
- [ ] The response shape of every API route matches the generic type of its corresponding hook
- [ ] Wrapped responses ({ items: [...] }) are unwrapped in the hook
- [ ] snake_case ↔ camelCase conversions are applied consistently
- [ ] Immediate responses (202) and final result shapes are handled distinctly in the frontend
- [ ] Every API endpoint has a corresponding frontend hook that is actually called

#### Routing Coherence
- [ ] All href/router.push values in the code match actual page file paths
- [ ] Path validation accounts for route groups ((group)) being removed from the URL
- [ ] Dynamic segments ([id]) are filled with the correct parameters

#### State Machine Coherence
- [ ] All defined state transitions are executed in code (no dead transitions)
- [ ] All status updates in code are defined in the transition map (no unauthorized transitions)
- [ ] Transitions from intermediate states to final states are not missing
- [ ] Status values used in frontend branches (if status === "X") are actually reachable

#### Data Flow Coherence
- [ ] The mapping between DB schema field names and API response field names is consistent
- [ ] Frontend type definitions match API response field names
- [ ] null/undefined handling for optional fields is consistent on both sides
```

---

## 5. QA Agent Definition Template

The core sections to include in the QA agent of a build harness.

```markdown
---
name: qa-inspector
description: "QA verification specialist. Validates spec compliance, integration coherence, and design quality."
---

# QA Inspector

## Core Role
Verify implementation quality against the spec and **integration coherence across module boundaries**.

## Verification Priorities

1. **Integration Coherence** (highest) — boundary mismatches are the primary cause of runtime errors
2. **Functional Spec Compliance** — API / state machine / data model
3. **Design Quality** — colors / typography / responsiveness
4. **Code Quality** — unused code, naming conventions

## Verification Method: "Read Both Sides at Once"

Boundary verification must always be done by **opening both sides of the code simultaneously** for comparison:

| Verification Target | Left Side (Producer) | Right Side (Consumer) |
|--------------------|---------------------|----------------------|
| API response shape | NextResponse.json() in route.ts | fetchJson<T> in hooks/ |
| Routing | page file paths under src/app/ | href, router.push values |
| State transitions | STATE_TRANSITIONS map | .update({ status }) code |
| DB → API → UI | table column names | API response fields → type definitions |

## Team Communication Protocol

- Upon discovery, send a specific fix request to the responsible agent (file:line + how to fix)
- For boundary issues, notify **both** agents on either side
- Report to the leader: a verification report (distinguish passed / failed / unchecked items)
```

---

## Real-World Cases: Bugs Found in SatangSlide

All lessons in this guide are drawn from the actual bugs listed below:

| Bug | Boundary | Root Cause |
|-----|----------|-----------|
| `projects?.filter is not a function` | API → hook | API returns `{projects:[]}`, hook expects an array |
| All dashboard links return 404 | file path → href | `/dashboard/` prefix missing |
| Theme images not displayed | API → component | `thumbnailUrl` vs `thumbnail_url` |
| Theme selection not saved | API → hook | select-theme API exists, hook missing |
| Create page waits forever | state transition → code | `template_approved` transition code missing |
| `data.failedIndices` crash | immediate response → frontend | Background result accessed from immediate response |
| View slides after completion returns 404 | file path → href | `/projects/` → `/dashboard/projects/` |
