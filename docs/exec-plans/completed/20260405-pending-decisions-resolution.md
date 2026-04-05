# Exec Plan: Pending Decisions 결정 반영

> 생성: 2026-04-05
> 영역: `docs/proposal/`

## 목적

office-hours 세션에서 사용자가 확정한 pending decisions를 기획 문서 전반에 반영한다.

## 확정 결정 요약

| # | 항목 | 결정 |
|---|------|------|
| 1 | v1 보스 범위 | C — 처음부터 완전한 2페이즈 보스로 유지 |
| 2 | 무기/전리품 선호 연결 | A — 장착 무기 = 전리품 선호 항상 동일 |
| 3 | v1 경제 컷라인 | B + 커스텀 — 거래 제거, 수리+재련+귀속(창고 제약) 유지 |
| 6 | 무기 authoring 구조 | 커스텀 — Item_Table 기본 틀, Item_Weapon_Table 상속 구조 |
| 7 | 전리품/원정 모드 authoring | 커스텀 — Reward_Table(멀티맵) + Dungeon_Table FK 2컬럼 |
| 8 | 보스 코어 스탯 분리 | A — BossCombatProfileTable + BossStaggerRuleTable 분리 |
| 9 | 귀속 상태 계약 분리 | 커스텀 — PracticeLootFlag 제거, BindRule Enum만 템플릿에. 귀속 상태는 T_ DB 테이블. 기획 테이블(Item_Table) / DB 테이블(T_Item) 명명 분리 |
| 10 | UI 표현 계약 | 커스텀 — 별도 UI 매핑 테이블 없음. 텍스트 테이블(string_key→text) + UI 하드코딩 |

## 단계

- [x] 1. `pending-decisions.md` 갱신 (#1,2,3,6,7,8 ✅ 확정 처리)
- [x] 2. `main.md` — v1 보스 범위 2페이즈로 수정, 경제 컷라인 반영
- [x] 3. `ingame/boss_pattern.md` — 2페이즈 보스 구조 반영
- [x] 4. `outgame/economy.md` — 거래 제거, 귀속 창고 제약으로 반영
- [x] 5. `outgame/dungeon_entry.md` — 전리품 선호 = 장착 무기 자동 반영
- [x] 6. `common/item_data_table.md` — Item_Table + Item_Weapon_Table 상속 구조, Reward_Table 멀티맵 + Dungeon_Table FK 구조
- [x] 7. `ingame/encounter_data_tables.md` — PhaseCount 2로 갱신, v1 EXTENSION 패턴 포함 명시
- [x] 8. `pending-decisions.md` — #9, #10 ✅ 확정 처리
- [x] 9. `common/item_data_table.md` — PracticeLootFlag 제거, BindRule Enum, T_Item DB 테이블 추가, ItemUiStateTable 제거, UI_Text_Table 명세, T_ 명명 규칙 추가

## 영향 경로

- `docs/proposal/pending-decisions.md`
- `docs/proposal/common/item_data_table.md`

## 완료 기준

- [x] pending-decisions.md에 #1~3, #6~8이 ✅ 확정으로 기록된다.
- [x] 각 proposal 문서가 #1~8 결정과 일치한다.
- [x] pending-decisions.md에 #9, #10이 ✅ 확정으로 기록된다.
- [x] item_data_table.md가 #9/#10 결정과 일치한다.
