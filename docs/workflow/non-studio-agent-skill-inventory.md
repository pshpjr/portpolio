# Non-Studio Agent/Skill Inventory (잔여 항목)

> 기준일: 2026-04-17
> 기준: Game Studio 역할(디렉터/리드/스페셜리스트, QA/릴리즈/콘텐츠 제작)과 **직접 연결되지 않는** 항목

## 분류 기준

- **A. Legacy 기획 비평 축**: 이전 기획 검토 하네스 유산
- **B. 저장소 운영/도구 인프라**: Game Studio 도메인이 아니라 리포 운영용
- **C. 문서 거버넌스/검토 인프라**: 문서 구조 관리용
- **D. 임시 브리지/호환 계층**: 외부 LLM/브릿지/특수 워크플로우

## 1) Non-Studio Agents

### A. Legacy 기획 비평 축

- 2026-04-17 기준 `plan-ceo/plan-eng-review` 연계 critic agents는 제거 완료

### B. 저장소 운영/도구 인프라 (즉시 삭제 금지)

- `worker`
- `reviewer`
- `scaffolder`
- `server-implementer`
- `codegen-specialist`
- `harness-improver`
- `tradeoff-moderator`
- `architecture-evaluator`

### C. 문서 거버넌스/검토 인프라

- `docs-reviewer`
- `docs-structure-reviewer`
- `docs-consistency-reviewer`
- `docs-context-reviewer`
- `design-author`
- `data-table-reviewer`

### D. 브리지/호환 계층

- `opencode-reviewer`

## 2) Non-Studio Skills

### A. 저장소 운영/도구 인프라 (즉시 삭제 금지)

- `codex-delegate`
- `codex-review`
- `opencode-review`
- `harness`
- `harness-improvement`
- `gitbutler`

### B. 문서/계획 거버넌스

- `doc-governance`
- `tradeoff-resolution`
- `architecture-eval`
- `performance-analysis`

### C. 계측/메모리 보조 계층

- `hermes-compress`
- `hermes-insights`
- `hermes-memory`
- `hermes-persona`
- `hermes-search`
- `hermes-traj`

## 3) 정리 제안

1. **1차 제거 후보**: (완료) Legacy 기획 비평 축 agent 제거
2. **2차 통합 후보**: `tradeoff-resolution`, `architecture-eval`
3. **유지 필수**: `doc-governance`, `codex-delegate`, `harness-improvement`, `opencode-review`
4. **보류**: `worker/reviewer/scaffolder`는 Studio 전환 완료 전까지 fallback으로 유지

## 4) 다음 액션

- 원하시면 다음 단계에서 위 목록 중 **1차 제거 후보만 실제 삭제 PR**로 분리해 진행 가능
- 동시에 `.codex/agents/` wrapper와 참조 문서를 함께 정리해야 안전함

## 5) Game Studio 원본 보존 검증 (2026-04-17)

- 검증 커맨드로 `Claude-Code-Game-Studios-main/.claude/` 대비 현재 루트 `.claude/`를 비교한 결과:
  - 누락 agent: **0건**
  - 누락 skill: **0건**
- 즉, 이번 정리는 Game Studio 원본 자산 삭제가 아니라, legacy non-studio 계층 정리다.
- `.codex/agents/` wrapper 삭제는 **대상 canonical agent 자체를 삭제한 경우에만** 동반 수행했다.
