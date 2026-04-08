# PR 제출 체크리스트

> PR을 열기 전 반드시 통과해야 하는 검사 목록입니다.

---

## 사전 검사 명령

```bash
# 1. 문서 링크/plan 검사
python tools/doc_check.py

# 2. 인코딩 검사
python tools/check_encoding.py

# 3. 서버 의존관계 검사
python tools/check_layers.py --root server/src --layer-order types,config,core,service,network,runtime

# 4. 라이브러리 의존관계 검사
python tools/check_layers.py --root Lib --layer-order include,src,tests
```

- 앱별 빌드와 테스트 명령은 각 앱의 workflow 문서를 따른다.
- 공용 Python 검사 도구 규칙은 [../conventions/tooling.md](../conventions/tooling.md)를 따른다.
- 저장소 텍스트 파일은 UTF-8 without BOM으로 유지한다.

---

## PR 설명 필수 항목

- **무엇을 했나**: 변경 내용 요약
- **왜 했나**: 동기, 설계 결정
- **영향 범위**: 수정된 레이어/도메인
- **테스트**: 어떻게 검증했나
