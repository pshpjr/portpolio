# Commands

## 기존 검사

```powershell
python tools/doc_check.py
python tools/check_encoding.py
python tools/check_layers.py --root server/src --layer-order types,config,core,service,network,runtime
python tools/check_layers.py --root Lib --layer-order include,src,tests
```

## 권장 추가 검사

```powershell
python server/tools/check_doc_links.py
python server/tools/check_doc_overlap.py
python server/tools/check_context_budget.py
python server/tools/check_doc_owners.py
```

## 사용 기준

- 문서를 수정했다면 `python tools/doc_check.py`를 반드시 실행한다.
- 링크 수정, 파일 이동, 인덱스 갱신: `check_doc_links.py`
- 문서 역할 분리와 중복 감시: `check_doc_overlap.py`, `check_doc_owners.py`
- 진입 문서 비대화 감시: `check_context_budget.py`
- 전체 문서 PR: `doc_check.py`에서 위 검사를 묶어 실행
