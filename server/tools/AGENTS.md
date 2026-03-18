# AGENTS.md
> Python 도구 작업 진입점.

## 이 디렉터리에서 먼저 확인할 것

1. `../AGENTS.md`
2. `../../docs/conventions/tooling.md`
3. `../docs/workflow/agent-workflow.md`
4. 필요하면 `../docs/workflow/verification.md`

## 도구 책임

- 검사 스크립트는 규칙 위반을 빠르게 드러내야 한다.
- 코드 생성 스크립트는 재현 가능해야 한다.
- 공용 문서/인코딩/레이어 검사는 루트 `tools/` 기준과 같은 동작을 유지해야 한다.

## 변경 시 체크

- 출력과 종료 코드가 자동화 친화적인지 확인
- 경로 가정이 `server/` 루트와 저장소 루트 모두에서 일관적인지 확인
- README나 관련 문서 갱신이 필요한지 검토
