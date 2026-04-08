# AGENTS.md
> 서버 앱용 에이전트 진입점입니다.
> 이 문서는 부트스트랩과 라우팅만 담당합니다. 상세 규칙은 링크된 문서의 단일 소스를 따릅니다.

---

## 프로젝트 요약

1~4인 인스턴스 던전 RPG용 게임 서버 포트폴리오 프로젝트.

상위 제품 기획과 공용 문서는 루트 [`../docs/`](../docs/) 아래에서 관리한다.

---

## 세션 시작 순서

1. 이 파일을 읽는다.
2. [docs/INDEX.md](./docs/INDEX.md)에서 작업 유형에 맞는 문서를 찾는다.
3. [docs/exec-plans/active/INDEX.md](./docs/exec-plans/active/INDEX.md)를 읽고 관련 active plan을 찾는다.
4. 관련 plan이 없으면 새 exec-plan을 먼저 작성하고 active index에 등록한다.
5. exec-plan 초안을 사용자에게 검토받기 전에는 구현 파일을 수정하지 않는다.
6. 수정 경로가 정해졌다면 [docs/context-map.md](./docs/context-map.md)와 해당 디렉터리의 로컬 `AGENTS.md`를 읽는다.

## 작업 시작 게이트

- 서버의 모든 작업은 크기와 무관하게 exec-plan 작성으로 시작한다.
- exec-plan 또는 사용자 검토 없이 코드, 빌드 설정, 테스트, 스크립트를 수정하면 안 된다.
- 기존 active plan이 있으면 이어받고, 없으면 새 plan을 만든다.
- 사용자가 plan 생략을 요청해도 서버 작업에서는 생략하지 않는다.

---

## 반드시 확인할 불변식

- 레이어 규칙의 단일 소스는 [ARCHITECTURE.md](./ARCHITECTURE.md)다.
- 워크플로, 검증, PR 전 체크리스트의 단일 소스는 [docs/workflow/agent-workflow.md](./docs/workflow/agent-workflow.md)다.
- 공용 설계 철학의 단일 소스는 루트 [../docs/](../docs/) 아래 문서이고, 공용 C++ 규칙은 [docs/conventions/cpp-style.md](./docs/conventions/cpp-style.md)다.
- 상위 기획이 필요할 때만 루트 [../docs/AGENTS.md](../docs/AGENTS.md)에서 관련 기획서를 찾아 읽는다.

레이어 요약:

```
Types -> Config -> Core -> Service -> Network -> Runtime
```

오른쪽 레이어를 왼쪽 레이어가 참조하면 안 된다.

---

## 빠른 라우팅

- C++ 코드 수정: [docs/conventions/cpp-style.md](./docs/conventions/cpp-style.md)
- Python 툴 수정: [../docs/conventions/tooling.md](../docs/conventions/tooling.md)
- 레이어/도메인 판단: [ARCHITECTURE.md](./ARCHITECTURE.md)
- 작업 절차, exec-plan, 검증: [docs/workflow/agent-workflow.md](./docs/workflow/agent-workflow.md)
- 제품 목표, 범위: [docs/design/overview.md](./docs/design/overview.md)
- 설계 원칙: [../docs/design/core-beliefs.md](../docs/design/core-beliefs.md)

---

## 디렉터리 진입점

- `src/03_core/` -> `src/03_core/AGENTS.md`
- `src/04_service/` -> `src/04_service/AGENTS.md`
- `tools/` -> `tools/AGENTS.md`
- `tests/` -> `tests/AGENTS.md`

로컬 `AGENTS.md`가 있으면 그 디렉터리 작업에서는 해당 문서를 우선한다.
