# Docs Index
> 작업 유형별 문서 진입점.
> 새 세션에서는 이 파일로 필요한 문서만 선택해서 읽는다.

## 항상 읽는 문서

- [../AGENTS.md](../AGENTS.md): 서버 앱 진입점
- [../ARCHITECTURE.md](../ARCHITECTURE.md): 레이어 불변식
- [workflow/agent-workflow.md](./workflow/agent-workflow.md): 작업 절차와 검증

## 작업 유형별 문서

- 제품 목표와 범위 확인:
  [design/overview.md](./design/overview.md)
- 설계 철학과 의사결정 기준 확인:
  [design/core-beliefs.md](./design/core-beliefs.md)
- C++ 구현 규칙 확인:
  [conventions/cpp-style.md](./conventions/cpp-style.md)
- Python 툴링 규칙 확인:
  [conventions/tooling.md](./conventions/tooling.md)
- 레이어 요약만 빠르게 확인:
  [architecture/layers.md](./architecture/layers.md)
- 상세 검증 명령 확인:
  [workflow/verification.md](./workflow/verification.md)
- 외부 라이브러리 확인:
  [architecture/dependencies.md](./architecture/dependencies.md)
- 품질 등급 참조:
  [quality/grades.md](./quality/grades.md)
- 저장소 구조 참조:
  [reference/project-layout.md](./reference/project-layout.md)

## 상황별 읽기 규칙

- `src/` 코드를 수정할 때는 `ARCHITECTURE.md`와 해당 하위 디렉터리 `AGENTS.md`를 읽는다.
- `tools/`를 수정할 때는 `conventions/tooling.md`와 `tools/AGENTS.md`를 읽는다.
- `tests/`를 수정할 때는 `tests/AGENTS.md`를 읽는다.
- `data/`를 수정할 때는 해당 데이터 디렉터리의 `README.md`를 읽는다.
- 활성 작업이 현재 변경과 직접 관련 있을 때만 해당 exec-plan 본문을 읽는다.
