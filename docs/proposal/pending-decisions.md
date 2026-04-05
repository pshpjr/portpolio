# Pending Decisions

> 기획서를 보강하면서 남은 사용자 결정 항목만 모아 둔 문서입니다.
> 이미 확정된 항목(예: `무기 변경 가능`)은 여기 다시 올리지 않습니다.

현재 문서 작업은 아래 `작업 기본안`을 기준으로 계속 진행한다.
사용자가 나중에 다른 안을 고르면 해당 문서만 국소 수정하는 방식으로 반영한다.

---

## ~~1. v1 보스 범위~~ ✅ 확정

**확정: C — 처음부터 완전한 2페이즈 보스로 유지.**

- `혼식 주술사`는 완전한 2페이즈 구조로 설계한다.
- 1페이즈: 읽기 쉬운 코어 패턴 + 무력화 딜 타임.
- 2페이즈: 체력 임계치 도달 시 전환, 강화된 패턴 + 협동 기믹 세트.
- 복잡한 소환 압박 강화, 추가 기믹 확장은 v1 이후로 미룰 수 있다.

---

## ~~2. 무기 변경과 전리품 선호의 연결 방식~~ ✅ 확정

**확정: A — 현재 장착 무기와 전리품 선호를 항상 동일하게 묶는다.**

- 던전 입장 시 현재 장착 무기가 전리품 선호 무기로 자동 결정된다.
- 별도 선택 UI 없음. 단순하고 설명 부담이 없다.

---

## ~~3. v1 경제 컷라인~~ ✅ 확정

**확정: B + 커스텀 — 유저 거래 제거, 수리 + 제한적 재련 + 귀속 유지.**

- v1에서는 유저 간 거래를 제거한다.
- 귀속 시스템은 유지하되 **창고 관련 제약**으로 범위를 한정한다.
  - 예: 인던 드롭 아이템은 계정 귀속 → 캐릭터 간 공유 창고로 이동 가능, 외부 거래 불가.
- 이후 거래 시스템이 추가될 때 귀속 규칙을 확장한다.
- `item_data_table.md`의 `TradeLimitCount` 컬럼은 제거하거나 v2 예약 처리한다.

---

## ~~4. 원정 모드 이원화 유지 여부~~ ✅ 확정

**확정: `정화 원정`과 `정찰 원정` 2모드 유지.**

`item_data_table.md`의 `DungeonRewardProfileTable`, `DropTable.RewardMode`, `LootPreferenceBiasTable.Mode`에 `PURIFICATION` / `SCOUTING` 구분이 유지된다.

---

## ~~5. v1 장비 슬롯 구성 고정~~ ✅ 확정

**확정: `무기 1 + 방어구 3(상의/하의/장갑) + 장신구 3(목걸이/반지 2)` = 총 7슬롯.**

- `EquipSlot` enum: `WEAPON`, `CHEST`, `LEGS`, `HANDS`, `NECK`, `RING_L`, `RING_R`
- `main.md`와 `item.md`를 동일 구조로 정렬 완료.
- `item_data_table.md` EquipSlot 반영 완료.

---

## ~~6. 무기 authoring 단일 소스 구조~~ ✅ 확정

**확정: 커스텀 — Item_Table 기본 틀 + Item_Weapon_Table 상속 구조.**

- `Item_Table`: 모든 아이템 공통 메타 (id, name, type, icon, grade, equip_slot, bind_policy 등).
- `Item_Weapon_Table`: `Item_Table`의 앞부분(공통 컬럼)과 동일한 구조를 먼저 두고, 그 뒤에 무기 전용 데이터(weapon_type, base_atk, stagger_add 등)를 붙인다.
- C++ 구조체 상속처럼 `ItemBase` → `ItemWeapon` 관계로 읽으면 된다.
- 소비형/방어구/장신구는 `Item_Table`만 사용하거나 각자 동일 패턴으로 확장한다.

---

## ~~7. 전리품 선호와 원정 모드의 authoring 결합 방식~~ ✅ 확정

**확정: 커스텀 — Reward_Table(멀티맵) + DungeonTable FK 2컬럼.**

- `Reward_Table`: `reward_id`는 고유 PK가 아닌 그룹 키(멀티맵). 동일 `reward_id`에 여러 행이 존재하며, 각 행이 보상 그룹 내 개별 드롭 항목을 나타낸다.
- `Dungeon_Table`: `purification_reward_id`, `scouting_reward_id` 두 컬럼으로 `Reward_Table.reward_id`를 참조한다.
- 정화/정찰 각각 독립적인 보상 그룹을 가질 수 있으며, 공유도 가능하다.

---

## ~~8. 보스 코어 스탯과 페이즈 규칙의 테이블 분리 방식~~ ✅ 확정

**확정: A — BossCombatProfileTable + BossStaggerRuleTable 분리.**

- `BossCombatProfileTable`: 보스 고유 전투 수치 (MaxHP, Defense, EnrageTime 등).
- `BossStaggerRuleTable`: 무력화 게이지, 상태 저항, 다운 후처리 규칙.
- `BossPatternTable`은 패턴/전조/체크포인트 authoring 전용으로 유지한다.
- 서버 로딩과 밸런스 조정 경계가 분리된다.

---

## ~~9. 아이템 귀속/거래 상태 계약을 어디까지 authoring 테이블로 끌어올릴지~~ ✅ 확정

**확정: 커스텀 — BindRule Enum을 기획 테이블에, 현재 귀속 상태는 T_ DB 테이블에.**

- 기획 테이블(`Item_Table`, `Item_Weapon_Table`)에는 `bind_rule` Enum만 둔다.
  - `BindRule`: `NONE`, `BIND_ON_ACQUIRE`, `BIND_ON_EQUIP` (v1 드롭 아이템은 `BIND_ON_ACQUIRE`)
- `PracticeLootFlag` 제거. 연습 보상 표시는 별도 UI 로직으로 처리하지 않는다.
- 현재 귀속 상태(`bind_state`)와 거래 잔여 횟수(`trade_count`)는 DB 테이블 `T_Item`에 런타임 컬럼으로 둔다.
- 기획 데이터 테이블 이름: `Item_Table` 형태 (접두사 없음).
- DB 테이블 이름: `T_` 접두사 (예: `T_Item`, `T_Account`, `T_Character`).
- 아이템 정보 UI에는 `귀속 상태 텍스트`와 `거래 횟수`를 표시한다. (v1은 거래 불가 = 0)

---

## ~~10. 전투/입장 UI 표현 계약 분리 방식~~ ✅ 확정

**확정: 커스텀 — 별도 UI 매핑 테이블 없음. 텍스트 테이블 + UI 하드코딩.**

- 별도 UI 표현 계약 테이블(`SkillUiPresentationTable`, `EntryBlockReasonTable` 등)은 만들지 않는다.
- 출력 텍스트는 `UI_Text_Table`(string_key → localized text)에서 관리한다.
- UI 코드에서 game state → string_key 매핑을 하드코딩한다. 매핑은 수정될 일이 없으므로 테이블화 불필요.
- `ItemUiStateTable`은 단순화하거나 제거하고, 텍스트 키만 `UI_Text_Table`로 참조한다.
