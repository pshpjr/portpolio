# Pending Decisions

> 기획서를 보강하면서 남은 사용자 결정 항목만 모아 둔 문서입니다.
> 이미 확정된 항목(예: `무기 변경 가능`)은 여기 다시 올리지 않습니다.

현재 문서 작업은 아래 `작업 기본안`을 기준으로 계속 진행한다.
사용자가 나중에 다른 안을 고르면 해당 문서만 국소 수정하는 방식으로 반영한다.

---

## 1. v1 보스 범위

작업 기본안:
- A. v1은 1페이즈 중심으로 고정

현재 초안:
- `혼식 주술사`는 1페이즈 중심의 읽기 쉬운 보스전으로 정리한다.
- 2페이즈 확장, 복잡한 강제 타게팅, 소환 압박 강화는 후속 확장으로 미룬다.

검토할 선택지:
- A. v1은 지금처럼 1페이즈 중심으로 고정
- B. 짧은 1.5페이즈 전환 연출만 추가
- C. 처음부터 완전한 2페이즈 보스로 유지

영향:
- A는 완성 가능성과 가독성이 가장 높다.
- C는 기억점은 늘지만 구현/밸런싱 비용이 크게 오른다.

---

## 2. 무기 변경과 전리품 선호의 연결 방식

작업 기본안:
- B. 입장 준비 구간에서 장착 무기와 전리품 선호를 별도로 선택

현재 초안:
- 마을 또는 입장 준비 구간에서 무기를 바꿀 수 있다.
- 던전 입장 시 현재 무기 프리셋과 `전리품 선호 무기`를 함께 잠근다.

검토할 선택지:
- A. 현재 장착 무기와 전리품 선호를 항상 동일하게 묶는다
- B. 입장 준비 구간에서 장착 무기와 전리품 선호를 별도로 선택한다
- C. 마을에서만 별도 전리품 선호를 설정하고 던전마다 유지한다

영향:
- A는 단순하지만 파밍 자유도가 낮다.
- B는 가장 유연하지만 UI와 설명 부담이 늘어난다.

---

## 3. v1 경제 컷라인

작업 기본안:
- A. `수리 + 제한적 재련 + 보조적 유저 거래` 유지

현재 초안:
- `수리 + 제한적 재련 + 보조적 유저 거래`를 유지한다.
- `정찰 원정` 보상은 연습/저가치 보상으로 제한한다.

검토할 선택지:
- A. 현재 초안 유지
- B. v1에서는 유저 거래를 빼고 `개인 드롭 + 수리 + 재련`만 남긴다
- C. 재련은 남기되 거래와 귀속 시스템은 후속 단계로 미룬다

영향:
- A는 시스템 설명은 풍부하지만 운영 규칙이 늘어난다.
- B와 C는 포트폴리오 컷라인을 더 선명하게 만든다.
- 이 결정은 `item_data_table.md`의 `TradeLimitCount`, `ReforgeCostTable`, `ItemUiStateTable` 유지 범위에도 직접 영향을 준다.

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

## 6. 무기 authoring 단일 소스 구조

작업 기본안:
- B. `ItemTemplateTable`는 공통 아이템 메타, `WeaponTable`은 전투 전용 메타로 분리하고 FK로 연결

현재 초안:
- `ItemTemplateTable`와 `WeaponTable`가 모두 무기 메타를 일부 갖고 있다.
- 거래/툴팁/아이콘 같은 공통 메타와 전투 참조 메타의 정본 위치가 아직 완전히 닫혀 있지 않다.

검토할 선택지:
- A. 무기 관련 authoring을 `WeaponTable` 하나로 최대한 모은다
- B. `ItemTemplateTable`는 공통 메타, `WeaponTable`은 전투 전용 메타로 분리하고 FK로 연결한다
- C. `ItemTemplateTable`를 정본으로 두고 `WeaponTable`는 최소 전투 참조 데이터만 남긴다

영향:
- A는 단순해 보이지만 소비형/방어구/장신구와의 공통 처리 경계가 흐려질 수 있다.
- B는 역할 분리가 가장 명확하지만 FK와 문서 보강이 추가로 필요하다.
- C는 인벤토리/경제 쪽은 단순해지지만 전투 전용 메타가 과도하게 축소될 수 있다.

---

## 7. 전리품 선호와 원정 모드의 authoring 결합 방식

작업 기본안:
- B. 던전/원정 모드별 보상 프로필을 두고, 전리품 선호 bias는 그 프로필이 참조한다

현재 초안:
- `DropTable.RewardMode`와 `LootPreferenceBiasTable`가 따로 존재하지만, 어느 던전/보상 흐름이 어떤 bias 규칙을 쓰는지 연결이 약하다.

검토할 선택지:
- A. 현재처럼 각 드롭 행에 `RewardMode`를 직접 둔다
- B. `DungeonRewardProfileTable` 같은 보상 프로필을 두고 `RewardMode`와 bias를 거기서 묶는다

영향:
- A는 단순하지만 드롭 행이 늘수록 규칙 중복이 커진다.
- B는 구조가 늘지만 서버/클라/기획이 같은 보상 규칙 단위를 공유하기 쉽다.
- 이 선택은 `item_data_table.md`의 `PracticeLootFlag`, `DropTable.RewardMode`, `LootPreferenceBiasTable.Mode`와 직접 연결된다.
- 단, `## 4. 원정 모드 이원화 유지 여부`에서 단일 모드로 정리되면 이 결정은 단일 보상 프로필 구조로 자동 축소된다.

---

## 8. 보스 코어 스탯과 페이즈 규칙의 테이블 분리 방식

작업 기본안:
- A. `BossPatternTable`와 별도로 `BossCombatProfileTable` 계열을 둔다

현재 초안:
- `boss_pattern.md`에 `MaxHP`, `Defense`, `StaggerGauge`, `StatusResistance`, `EnrageTime`, 무력화 후처리 규칙이 직접 적혀 있다.
- `encounter_data_tables.md`에는 패턴/전조/체크포인트는 있지만, 보스 코어 전투 프로필과 페이즈/저항 규칙 테이블은 없다.

검토할 선택지:
- A. `BossCombatProfileTable`과 `BossStaggerRuleTable`을 추가해 패턴 테이블과 분리한다
- B. `BossPatternTable`에 보스 공통 수치를 억지로 포함한다
- C. v1은 마크다운 문서에만 두고 테이블화는 후속으로 미룬다

영향:
- A는 서버 로딩과 밸런스 조정 경계가 가장 명확하다.
- B는 패턴 데이터와 보스 공통 규칙이 섞여 변경 비용이 커진다.
- C는 구현 초기에 하드코딩 유혹이 커지고 data-driven 설명력이 약해진다.

---

## 9. 아이템 귀속/거래 상태 계약을 어디까지 authoring 테이블로 끌어올릴지

작업 기본안:
- B. authoring은 규칙만 두고, 현재 상태는 런타임 인스턴스 계약으로 분리한다

현재 초안:
- `item_data_table.md`에는 `TradeLimitCount`, `PracticeLootFlag`, `BindOnReforge`는 있지만 현재 귀속 상태, 귀속 사유, 거래 차단 사유 계약은 없다.
- `item.md`, `economy.md`는 `계정 귀속`, `거래 가능 횟수 0`, `연습 보상` UI를 요구한다.

검토할 선택지:
- A. `ItemTemplateTable`에 귀속 상태까지 직접 넣는다
- B. authoring은 `TransferPolicy`, `BindOnReforge` 같은 규칙만 두고, 현재 상태는 서버 인스턴스 계약으로 분리한다
- C. 모든 귀속/거래 상태를 구현 문서로 넘기고 기획 테이블에서는 다룰지 않는다

영향:
- B가 static rule과 runtime state를 가장 깔끔하게 분리한다.
- A는 템플릿과 인스턴스 상태가 섞일 위험이 있다.
- C는 UI/기획 문서가 요구하는 상태 표현 근거가 약해진다.

---

## 10. 전투/입장 UI 표현 계약 분리 방식

작업 기본안:
- B. 게임플레이 테이블은 판정/규칙 위주로 두고, UI 표현 키는 별도 표현 계약 테이블로 분리

현재 초안:
- `combat.md`, `combat_ui_wireframe.md`, `dungeon_entry.md`는 패링/인터럽트/무력화/차단 사유/잠금 상태 같은 UI를 약속한다.
- 하지만 `skill_data_table.md`, `encounter_data_tables.md`, `item_data_table.md`에는 아이콘/문구/스타일/미리보기 키 계약이 충분하지 않다.

검토할 선택지:
- A. 기존 스킬/전투/아이템 테이블에 UI key 컬럼을 계속 직접 추가한다
- B. `SkillUiPresentationTable`, `CombatFeedbackPresentationTable`, `EntryBlockReasonTable` 같은 별도 표현 계약 테이블로 분리한다
- C. v1에서는 코드 상수로 처리하고 기획 문서에는 최소 키만 남긴다

영향:
- A는 빠르게 문서를 채울 수 있지만 게임플레이와 표현 책임이 섞인다.
- B는 클라/서버/기획 경계가 가장 선명하지만 테이블 수가 늘어난다.
- C는 구현은 빠를 수 있지만 문서 우선, 데이터 드리븐 방향이 약해진다.
