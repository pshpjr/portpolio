# Active Exec Plans
> `docs/proposal/` 영역의 활성 작업 계획 인덱스입니다.
> 새 세션에서는 이 파일만 먼저 읽고, 관련 있는 plan 본문만 엽니다.

## 현재 활성 계획

### `20260330-combat-controls-and-view.md`

- 목적: 전투 기획 문서에 권장 시점, 기본 입력 체계, 스킬 사용 방식, 무기별 조작 예시를 추가
- 주 영향 경로: `docs/proposal/ingame/combat.md`, `docs/proposal/common/skill.md`
- 이 plan을 읽어야 하는 경우:
  - 전투 시점과 기본 조작 체계를 정할 때
  - `WASD` 기반 스킬 운용 구조를 문서화할 때
  - 무기별 입력 예시와 서버 2D 판정 전제를 맞출 때

### `20260330-combat-stat-foundation.md`

- 목적: 전투 스탯 목록과 전투 공식 기반 문서를 추가해 후속 밸런스/엑셀 작업의 기준을 마련
- 주 영향 경로: `docs/proposal/ingame/combat_stats.md`, `docs/proposal/ingame/combat_formulas.md`, `docs/proposal/ingame/combat.md`, `docs/proposal/main.md`
- 이 plan을 읽어야 하는 경우:
  - 전투 스탯 체계를 처음 정의할 때
  - 전투 수식의 기본 구조를 합의할 때
  - 밸런스 엑셀 전 단계에서 컬럼과 계산식을 정리할 때

### `20260316-proposal-restructure.md`

- 목적: `docs/proposal/`을 에이전트 친화적인 허브/세부 문서 구조로 재정리하고, 진행 중 작업 문서 체계를 추가
- 주 영향 경로: `docs/proposal/AGENTS.md`, `docs/proposal/main.md`, `docs/proposal/exec-plans/`, `docs/proposal/common/`, `docs/proposal/ingame/`, `docs/proposal/outgame/`
- 이 plan을 읽어야 하는 경우:
  - 기획서 구조, 허브 문서, 하위 문서 템플릿을 수정할 때
  - 다른 에이전트에게 기획 문서 정리 작업을 넘길 때
  - 문서 간 중복 제거와 역할 분리를 이어서 진행할 때

## 운영 규칙

- 현재 작업과 직접 무관한 활성 plan은 열지 않는다.
- 작업 중 결정한 내용은 관련 plan의 진행 상황과 설계 기록에 반영한다.
- 완료된 plan은 `completed/`로 이동한다.
