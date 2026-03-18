# AGENTS.md
> 저장소 공용 도구 작업 진입점.

## 이 디렉터리에서 먼저 확인할 것

1. `../AGENTS.md`
2. `../docs/AGENTS.md`
3. `../docs/conventions/tooling.md`
4. 필요하면 `../docs/workflow/agent-workflow.md`

## 도구 책임

- 공용 검사 스크립트는 서버와 `Lib/` 모두에서 재사용 가능해야 한다.
- 출력과 종료 코드는 자동화 친화적이어야 한다.
- 특정 앱 전용 로직을 공용 도구에 섞지 않는다.
