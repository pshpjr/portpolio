# Docs Index
> `Lib/` 작업 유형별 문서 진입점.
> 새 세션에서는 이 파일로 필요한 문서만 선택해서 읽는다.

## 항상 읽는 문서

- [../AGENTS.md](../AGENTS.md): `Lib/` 영역 진입점
- [../../docs/design/core-beliefs.md](../../docs/design/core-beliefs.md): 공용 설계 철학
- [../../server/docs/conventions/cpp-style.md](../../server/docs/conventions/cpp-style.md): 공용 C++ 규칙

## 작업 유형별 문서

- 공용 라이브러리 개요:
  [../README.md](../README.md)
- 경로별 최소 문서:
  [context-map.md](./context-map.md)
- 활성 실행 계획:
  [exec-plans/active/INDEX.md](./exec-plans/active/INDEX.md)
- 공용 Python 도구 규칙:
  [../../docs/conventions/tooling.md](../../docs/conventions/tooling.md)

## 상황별 읽기 규칙

- `include/`를 수정할 때는 공개 API 안정성과 의존성 방향을 먼저 확인한다.
- `src/`를 수정할 때는 공개 헤더와 테스트 영향 범위를 함께 확인한다.
- `tests/`를 수정할 때는 어떤 공용 계약을 검증하는지 명확히 한다.
