# Combat Stat Foundation

## 목표
기존 전투 기획 문서를 바탕으로 전투 스탯 목록과 핵심 전투 공식 문서를 추가해, 이후 밸런스 수치와 엑셀 데이터를 같은 구조 위에서 정리할 수 있게 한다.

## 범위
- `docs/proposal/ingame/combat_stats.md`
- `docs/proposal/ingame/combat_formulas.md`
- `docs/proposal/ingame/combat_data_tables.md`
- `docs/proposal/ingame/combat.md`
- `docs/proposal/main.md`
- `docs/proposal/exec-plans/active/INDEX.md`

## 완료 기준
- [x] 전투 스탯 문서가 추가된다.
- [x] 전투 공식 문서가 추가된다.
- [x] 전투 데이터 테이블 문서가 추가된다.
- [x] 관련 허브 문서와 링크가 연결된다.
- [x] 문서 검증 결과를 기록한다.
- [ ] 변경분만 선별 커밋한다.

## 진행 상황
- [x] 현재 proposal 문서와 active plan 인덱스를 확인했다.
- [x] 전투 스탯/공식 문서를 추가한다.
- [x] 레벨별 스탯 인덱스와 무기 테이블 구조를 문서화한다.
- [x] 반복 수치 테이블을 CSV와 재생성 스크립트로 분리한다.
- [x] 링크와 허브 문서를 갱신한다.
- [x] 전투 공식/스탯 시스템의 포트폴리오 범위를 재검토하고 `v1 코어`와 `후속 확장`을 분리했다.
- [ ] 문서 검증과 커밋을 수행한다.

## 설계 메모
- 수치값 자체보다 계산 구조를 먼저 고정한다.
- 무기 기반 정체성, 패링, 무력화, 속성 상호작용, 위협도 운용을 모두 표현할 수 있어야 한다.
- 방어, 저항, 무력화 저항은 감쇠 구조를 우선 사용해 값 폭주를 피한다.
- 포트폴리오 1차 범위는 `패링/카운터`, `무력화/다운`, `무기 정체성`, `기본 피해 공식`까지를 핵심으로 본다.
- 상태 이상 세부 규칙, 위협도 감쇠/예외, 다중 히트 패링 같은 항목은 문서에 남기더라도 후속 확장으로 격리한다.
- `BaseMoveSpeed`와 `MoveSpeedBonus`를 분리해 이동 속도 의미 충돌을 없앤다.
- 캐릭터 기본 스탯과 스킬/보스 데이터 필드를 분리해 v1 테이블 스키마를 먼저 고정한다.
- v1 이후 독립 후보 문서:
  - `combat_elements.md`: 속성 상성, 약점 보정, 속성별 추가 규칙
  - `combat_threat.md`: 위협도 감쇠, 힐/보호막 위협, 강제 타게팅 복귀 규칙
  - `combat_status_effects.md`: 상태 이상 축적, 지속시간, 저항 증가 규칙
  - `combat_parry_edgecases.md`: 다중 히트 패링, `Parryable` 태그 우선순위
  - `combat_control.md`: 제어 저항, 반복 제어 완화, 보스 면역 규칙
- 밸런스 초기 기준선은 `솔로 기준 TTK`, `적 타입별 HP/Defense/StaggerGauge`, `무기별 SkillCoeff/SkillStaggerCoeff/SkillGaugeCoeff`를 먼저 고정한 뒤 세부 스킬 값으로 내려간다.
- `python tools/doc_check.py`는 실행했지만, 이번 변경과 무관한 `.vcpkg-downloads/.../README.md -> CODE_OF_CONDUCT.md` 깨진 링크 1건 때문에 전체 실패로 보고되었다.
- 반복 행이 큰 전투 테이블은 문서 본문보다 `CSV + 생성 스크립트` 구조가 더 적합하다.
- `WeaponTable`은 템플릿/메타 중심, `WeaponStatTable`은 실제 수치 중심으로 분리하고, `WeaponTable`에는 강화 1 기준 요약 스탯만 중복 허용한다.
