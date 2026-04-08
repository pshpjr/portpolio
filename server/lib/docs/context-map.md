# Context Map
> `Lib/` 수정 경로 기준으로 읽어야 할 최소 문서를 정의한다.

## 기본 규칙

- 모든 작업은 `../AGENTS.md`를 시작점으로 한다.
- C++ 규칙은 `../../server/docs/conventions/cpp-style.md`를 따른다.
- 공용 검사 도구 규칙은 `../../docs/conventions/tooling.md`를 따른다.

## 경로별 필독 문서

| 수정 경로 | 필독 문서 | 비고 |
|---|---|---|
| `include/**` | `../../server/docs/conventions/cpp-style.md`, `../include/README.md` | 공개 헤더와 재사용성 우선 |
| `src/**` | `../../server/docs/conventions/cpp-style.md`, `../src/README.md` | 구현이 공개 API를 새지 않게 유지 |
| `tests/**` | `../../server/docs/conventions/cpp-style.md`, `../tests/README.md` | 공용 계약 검증 |
| `docs/**` | `../../docs/conventions/tooling.md` | 문서 역할과 공용 도구 경로 확인 |

## exec-plan 로드 규칙

- 먼저 `exec-plans/active/INDEX.md`만 읽는다.
- 현재 작업 범위와 겹치는 plan만 추가로 연다.
