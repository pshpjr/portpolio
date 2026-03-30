# V1 Slice And Item Stat Clarity

## 목표
`main.md`에 첫 완성본의 `v1 showcase slice`를 명시하고, 아이템/장비/전투 스탯 문서에서 장비 계층과 스탯 출처를 더 선명하게 정리한다.

## 범위
- `docs/proposal/main.md`
- `docs/proposal/common/item.md`
- `docs/proposal/ingame/combat_stats.md`
- `docs/proposal/ingame/combat_formulas.md`
- `docs/proposal/ingame/combat_data_tables.md`
- `docs/proposal/exec-plans/active/INDEX.md`
- `docs/proposal/exec-plans/active/20260330-v1-slice-and-item-stat-clarity.md`

## 완료 기준
- [x] `main.md`에 `v1 showcase slice`와 명시적 defer 항목이 추가된다.
- [x] `common/item.md`에 장비 슬롯, 아이템 등급/획득/귀속/재련 경계가 추가된다.
- [x] `combat_stats.md`에 스탯 출처와 최종 스탯 조립 규칙이 추가된다.
- [x] 필요 시 `combat_formulas.md`, `combat_data_tables.md`에 item/equipment 연동 책임이 반영된다.
- [x] `python ../../tools/doc_check.py` 검증 결과를 기록한다.
- [ ] 변경분만 선별 커밋한다.

## 진행 상황
- [x] `docs/proposal/main.md`, `common/item.md`, `ingame/combat_stats.md`, `ingame/combat_formulas.md`를 검토했다.
- [x] 기존 active plan 중 `combat-stat-foundation`, `game-design-agent-roundtable`와의 경계를 확인했다.
- [x] `v1 showcase slice`와 defer 범위를 허브 문서에 반영했다.
- [x] item/equipment/stat 문서의 부족한 책임 경계를 보강했다.
- [x] 문서 검증을 실행했다.
- [ ] 변경분만 선별 커밋한다.

## 설계 메모
- 허브 문서에는 "완성본으로 무엇을 보여주는가"를 먼저 고정해, 세부 문서의 확장 요소가 v1 범위를 흔들지 않게 해야 한다.
- 현재 `common/item.md`는 무기 중심 설명이 강하지만 장비 슬롯 구조, 방어구/장신구 역할, 옵션 소스, 귀속 전이 규칙이 압축돼 있어 장비 시스템 전체 그림이 약하다.
- 현재 `combat_stats.md`는 스탯 목록은 강하지만 "레벨/무기/장비옵션/버프가 어떤 순서로 합쳐지는가"가 분산돼 있다.
- 이번 변경은 구현 세부 DB 확정보다 기획 관점의 책임 경계와 v1 설명 가능성을 높이는 데 집중한다.
- `main.md`에는 `마을 1곳 + 던전 1종 + 보스 1종 + 무기 3종 + 최소 경제 루프 + 성능 리포트`를 첫 완성본 기준으로 고정했다.
- `common/item.md`에는 `무기 1 + 방어구 4 + 장신구 2` 슬롯 구조와 장비군별 역할, smart drop soft bias, 재련/귀속 경계를 명시했다.
- `combat_stats.md`와 `combat_formulas.md`에는 `레벨 -> 무기 -> 장비 고정 스탯 -> 랜덤 옵션 -> 임시 효과 -> 상한 적용`의 조립 순서를 추가했다.
- `combat_data_tables.md`에는 방어구/장신구가 v1에서 무기처럼 복잡한 강화 테이블이 아니라 `ItemTemplate + FixedStatSet + OptionPool` 조합으로 시작한다는 책임 경계를 추가했다.
- `python ../../tools/doc_check.py` 실행 결과 문서 검증은 통과했다.
