# AGENTS.md
> 테스트 디렉터리 작업 진입점.

## 이 디렉터리에서 먼저 확인할 것

1. `../AGENTS.md`
2. `../docs/workflow/agent-workflow.md`
3. `../ARCHITECTURE.md`
4. 필요하면 `../docs/workflow/verification.md`

## 테스트 배치 기준

- `unit/`: Types, Config, Core처럼 순수 로직 중심 검증
- `integration/`: 세션, 로더, 조립된 흐름 검증

## 변경 시 체크

- 새 테스트가 어느 레이어 책임을 검증하는지 명확한지 확인
- Core 변경인데 통합 테스트만 추가하는 실수를 피함
- 스캐폴드 테스트를 실제 회귀 방지 테스트로 대체할 수 있는지 검토
