# Document Roles

## Role Table

| Document Type | Role | Must Include | Must Not Include |
|---|---|---|---|
| `AGENTS.md` | Bootstrap and routing | Reading order, single-source links, directory entry points | Long background descriptions, detailed commands, status tables |
| `docs/INDEX.md` | Document selection guide | Entry points by task type | Re-explaining rules already in body documents |
| `docs/context-map.md` | Must-read document map by path | Mapping of paths to required documents | Long summaries of each document |
| `ARCHITECTURE.md` | Layer invariants | Dependency rules, responsibilities, key decision criteria | Implementation status tables, full trees, lengthy external library descriptions |
| `workflow/agent-workflow.md` | Work procedure | Start sequence, exec-plan rules, verification principles | Platform-specific detailed commands |
| `workflow/verification.md` | Detailed verification commands | Build/test/check commands, environment notes | Re-explaining work procedure |
| `design/*` | Goals and philosophy | Product scope, design principles | Path-based routing |
| `reference/*` | Reference information | Full trees, library tables, supplementary material | Mandatory pre-read rules |
| `exec-plans/active/INDEX.md` | Active plan summary | Plan name, affected paths, when to read | Duplicating plan body details |

## Role Determination Questions

- Can work begin from other single-source documents alone, without this document?
- Is this content a rule, a procedure, or reference material?
- If the same content appears in two or more documents, which one is the true owner?
