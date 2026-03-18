# Context Map
> 수정 경로 기준으로 읽어야 할 최소 문서를 정의한다.
> 불필요한 배경 문서 로드를 줄이는 것이 목적이다.

## 기본 규칙

- 모든 작업은 `../AGENTS.md`를 시작점으로 한다.
- 아키텍처 판단이 필요한 코드 변경은 `../ARCHITECTURE.md`를 읽는다.
- 작업 절차와 검증은 `workflow/agent-workflow.md`를 단일 소스로 본다.
- 공용 설계 철학은 `../../docs/design/core-beliefs.md`를 따른다.
- 공용 C++/Python 규칙은 `../../docs/conventions/` 아래 문서를 따른다.
- 아래 경로 규칙에 없는 경우, 가장 가까운 디렉터리의 `AGENTS.md` 또는 `README.md`를 우선 읽는다.

## 경로별 필독 문서

| 수정 경로 | 필독 문서 | 비고 |
|---|---|---|
| `src/01_types/**` | `../ARCHITECTURE.md`, `../../docs/conventions/cpp-style.md` | 타입 계층 불변식 중심 |
| `src/02_config/**` | `../ARCHITECTURE.md`, `../../docs/conventions/cpp-style.md` | 로더/스키마 경계 확인 |
| `src/03_core/**` | `../ARCHITECTURE.md`, `../../docs/conventions/cpp-style.md`, `../src/03_core/AGENTS.md` | 테스트 가능성과 순수 로직 유지 |
| `src/04_service/**` | `../ARCHITECTURE.md`, `../../docs/conventions/cpp-style.md`, `../src/04_service/AGENTS.md` | Core 조합 규칙 확인 |
| `tests/**` | `workflow/agent-workflow.md`, `../tests/AGENTS.md` | 테스트 레벨 선택 |
| `tools/**` | `../../docs/conventions/tooling.md`, `../tools/AGENTS.md` | Python 스타일과 실행 규칙 |
| `data/skills/**` | `../data/skills/README.md` | 런타임 핫리로드 대상 |
| `data/items/**` | `../data/items/README.md` | 아이템 정의 데이터 |
| `data/monsters/**` | `../data/monsters/README.md` | 몬스터 정의 데이터 |
| `docs/**` | `../../docs/design/core-beliefs.md`, `workflow/agent-workflow.md` | 서버 문서와 공용 문서의 경계 확인 |

## exec-plan 로드 규칙

- 먼저 `exec-plans/active/INDEX.md`만 읽는다.
- 아래 중 하나에 해당할 때만 개별 plan 본문을 연다.
- 현재 수정 경로가 그 plan의 범위와 겹친다.
- 현재 작업이 그 plan의 완료 기준을 직접 갱신한다.
- 설계 결정을 이어받아야 한다.
