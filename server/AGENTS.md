# AGENTS.md
> 서버 앱용 에이전트 진입점입니다.
> 이 문서는 부트스트랩과 라우팅만 담당합니다. 상세 규칙은 링크된 문서의 단일 소스를 따릅니다.

---

## 프로젝트 요약

1~4인 인스턴스 던전 RPG용 게임 서버 포트폴리오 프로젝트.

---

## 세션 시작 순서

1. 이 파일을 읽는다.
2. [docs/INDEX.md](./docs/INDEX.md)에서 작업 유형에 맞는 문서를 찾는다.
3. [docs/exec-plans/active/INDEX.md](./docs/exec-plans/active/INDEX.md)에서 현재 활성 계획이 자신과 관련 있는지 확인한다.
4. 수정 경로가 정해졌다면 [docs/context-map.md](./docs/context-map.md)와 해당 디렉터리의 로컬 `AGENTS.md`를 읽는다.

---

## 반드시 확인할 불변식

- 레이어 규칙의 단일 소스는 [ARCHITECTURE.md](./ARCHITECTURE.md)다.
- 워크플로, 검증, PR 전 체크리스트의 단일 소스는 [docs/workflow/agent-workflow.md](./docs/workflow/agent-workflow.md)다.
- 설계 철학과 제품 범위의 단일 소스는 [docs/design/](./docs/design/) 아래 문서다.

레이어 요약:

```
Types -> Config -> Core -> Service -> Network -> Runtime
```

오른쪽 레이어를 왼쪽 레이어가 참조하면 안 된다.

---

## 빠른 라우팅

- C++ 코드 수정: [docs/conventions/cpp-style.md](./docs/conventions/cpp-style.md)
- Python 툴 수정: [docs/conventions/tooling.md](./docs/conventions/tooling.md)
- 레이어/도메인 판단: [ARCHITECTURE.md](./ARCHITECTURE.md)
- 작업 절차, exec-plan, 검증: [docs/workflow/agent-workflow.md](./docs/workflow/agent-workflow.md)
- 제품 목표, 범위: [docs/design/overview.md](./docs/design/overview.md)
- 설계 원칙: [docs/design/core-beliefs.md](./docs/design/core-beliefs.md)

---

## 디렉터리 진입점

- `src/03_core/` -> `src/03_core/AGENTS.md`
- `src/04_service/` -> `src/04_service/AGENTS.md`
- `src/05_network/` -> `src/05_network/AGENTS.md`
- `tools/` -> `tools/AGENTS.md`
- `tests/` -> `tests/AGENTS.md`

로컬 `AGENTS.md`가 있으면 그 디렉터리 작업에서는 해당 문서를 우선한다.
