# Active Exec Plans
> `docs/proposal/` 영역의 활성 작업 계획 인덱스입니다.
> 새 세션에서는 이 파일만 먼저 읽고, 관련 있는 plan 본문만 엽니다.

## 현재 활성 계획

### `20260330-combat-formula-draft.md`

- 목적: 전투 스탯 초안을 바탕으로 데미지, 무력화, 속성, 상태 이상, 위협도 등 핵심 전투 공식의 초안 문서를 추가
- 주 영향 경로: `docs/proposal/ingame/combat_formulas.md`, `docs/proposal/ingame/combat.md`, `docs/proposal/ingame/combat_stats.md`, `docs/proposal/main.md`
- 이 plan을 읽어야 하는 경우:
  - 전투 수식 초안을 기획 문서에 처음 추가할 때
  - 엑셀 설계 전에 필요한 계산 항목과 입력 스탯 구조를 확인할 때
  - 구현 문서로 넘기기 전 기획 레벨의 계산 방향을 합의할 때

### `20260330-combat-stat-draft.md`

- 목적: 기존 기획서를 바탕으로 전투 스탯 초안 문서를 추가하고, 후속 엑셀/밸런스 정리에 필요한 스탯 목록을 정리
- 주 영향 경로: `docs/proposal/ingame/combat_stats.md`, `docs/proposal/ingame/combat.md`, `docs/proposal/main.md`, 필요 시 관련 전투 문서
- 이 plan을 읽어야 하는 경우:
  - 전투 스탯 항목을 기획 문서로 처음 정리할 때
  - 수치 밸런스 엑셀의 컬럼 구조를 잡기 전에 스탯 분류를 확인할 때
  - 전투 문서와 스킬/아이템 문서 사이의 참조를 보강할 때

### `20260330-combat-formula-stat-audit.md`

- 목적: 전투 공식과 스탯 시스템이 기획서에 어디까지 정의되어 있는지 조사하고, 후속 엑셀/기획 정리에 필요한 공백을 식별
- 주 영향 경로: `docs/proposal/ingame/combat.md`, `docs/proposal/common/skill.md`, `docs/proposal/common/item.md`, `docs/proposal/common/user.md`, 필요 시 관련 하위 문서
- 이 plan을 읽어야 하는 경우:
  - 전투 수식, 데미지 계산, 상태 이상 누적 규칙의 정의 수준을 확인할 때
  - 플레이어/장비 스탯 항목이 기획서에 명시되었는지 점검할 때
  - 전투 밸런스용 엑셀 또는 데이터 시트를 만들기 전에 근거 문서를 수집할 때

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
