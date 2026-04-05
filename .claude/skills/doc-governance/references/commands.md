# Commands

## Existing Checks

```powershell
python tools/doc_check.py
python tools/check_encoding.py
python tools/check_layers.py --root server/src --layer-order types,config,core,service,network,runtime
python tools/check_layers.py --root Lib --layer-order include,src,tests
```

## Recommended Additional Checks

```powershell
python server/tools/check_doc_links.py
python server/tools/check_doc_overlap.py
python server/tools/check_context_budget.py
python server/tools/check_doc_owners.py
```

## When to Use Each

- After modifying any document, always run `python tools/doc_check.py`.
- Link edits, file moves, index updates: `check_doc_links.py`
- Document role separation and duplication monitoring: `check_doc_overlap.py`, `check_doc_owners.py`
- Entry document bloat monitoring: `check_context_budget.py`
- Full documentation PR: `doc_check.py` bundles all of the above checks together
