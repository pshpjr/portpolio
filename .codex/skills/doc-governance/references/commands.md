# Commands

## 기존 검사

```powershell
python server/tools/doc_check.py
python server/tools/check_encoding.py
python server/tools/check_layers.py
```

## 권장 추가 검사

```powershell
python server/tools/check_doc_links.py
python server/tools/check_doc_overlap.py
python server/tools/check_context_budget.py
python server/tools/check_doc_owners.py
```

## 사용 기준

- 링크 수정, 파일 이동, 인덱스 갱신: `check_doc_links.py`
- 문서 역할 분리와 중복 감시: `check_doc_overlap.py`, `check_doc_owners.py`
- 진입 문서 비대화 감시: `check_context_budget.py`
- 전체 문서 PR: `doc_check.py`에서 위 검사를 묶어 실행
