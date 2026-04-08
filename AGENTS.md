# AGENTS.md
> 모노레포 루트 진입점입니다.
> 실제 앱 작업 전에는 대상 앱 디렉터리의 `AGENTS.md`를 먼저 읽으십시오.

---

## 현재 앱 구성

- `server/` — 게임 서버 프로젝트
- `Lib/` — 서버와 언리얼 클라이언트가 함께 쓰는 공용 라이브러리
- `client/` — 예정
- `dummyserver/` — 예정

## 상위 문서 구성

- `docs/` — 앱 바깥의 상위 문서 카테고리
- 기획서, 공용 워크플로, 루트 exec-plan은 `docs/` 아래에서 관리

---

## 작업 시작 순서

1. 작업이 앱 구현인지 상위 문서 작업인지 결정
2. 앱 구현이면 해당 앱 디렉터리의 `AGENTS.md`를 읽기
3. 상위 문서 작업이면 `docs/AGENTS.md`를 읽기
4. 해당 영역의 문서, exec-plan, 빌드/테스트 절차에 따라 작업

---

## 현재 기준

- 서버 작업: [server/AGENTS.md](./server/AGENTS.md)
- 공용 라이브러리 작업: [server/lib/AGENTS.md](./server/lib/AGENTS.md)
- 상위 문서 작업: [docs/AGENTS.md](./docs/AGENTS.md)
- 서버 아키텍처/설계/컨벤션 문서는 `server/` 하위에 있음
- 공용 라이브러리 코드는 `server/lib/` 하위에 있음 (서버 전용)
- 기획서와 루트 워크플로 문서는 `docs/` 하위에 있음
- 저장소 공용 skill/agent는 루트 `.codex/`, `.claude/` 하위에서 관리
- 루트는 모노레포 조정용이며, 영역별 상세 규칙은 각 진입 문서에서 관리
