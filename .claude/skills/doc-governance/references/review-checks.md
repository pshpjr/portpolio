# Review Checks

## Structure Checks

- Does the document contain content outside its own defined role?
- Has any duplicated description appeared that violates single-source ownership?
- Has detailed explanation crept up into a higher-level entry document?

## Routing Checks

- Is the new document discoverable from the appropriate location among `AGENTS.md`, `docs/INDEX.md`, and `context-map.md`?
- Are path-based workers directed to read only the documents they need?
- Has a new directory been created that lacks or has an insufficient local `AGENTS.md`?

## Consistency Checks

- Are links and relative paths valid?
- After moving a document, was a reference link left in the previous location?
- Are terminology and document names used consistently across multiple locations?
- Was `python tools/doc_check.py` actually run after modifying documents?

## Context Checks

- Has the number of mandatory pre-read documents grown?
- Have entry documents become verbose?
- Do lower-level documents repeat the content of higher-level documents verbatim?
