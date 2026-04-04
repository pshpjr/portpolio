# 아이템 / 드롭 데이터 테이블 구조

## 목적

아이템, 드롭, 전리품 선호 보정, 재련 비용 authoring에 필요한 테이블 구조와 책임 범위를 정의한다.
실제 행 데이터는 JSON/CSV로 관리하고, 이 문서는 컬럼 계약과 테이블 간 연결 규칙만 다룬다.

---

## 테이블 전체 관계 다이어그램

```
ItemTemplateTable.ItemId
  → DropTable.ItemId                    (드롭 가중치 조회)
  → ReforgeCostTable (Rarity 기준)      (재련 비용 조회)
  → ItemUiStateTable.UiStateTag         (UI 상태 배지 조회)
  → BaseStatSetTable.BaseStatSetId      (기본 스탯 조회)
  → OptionPoolTable.OptionPoolId        (옵션 풀 조회)

DropTable.DropTableId
  → DungeonRewardProfileTable.DefaultDropTableId  (보상 프로필 → 드롭)

DungeonRewardProfileTable
  ← DungeonId + RewardMode              (던전/원정 모드별 보상 정책 조회)
  → LootPreferenceBiasTable.BiasProfileId (전리품 선호 bias 참조)

WeaponTable.WeaponId
  → ItemTemplateTable.ItemId (ItemCategory = WEAPON)  (공통 아이템 메타 공유)
```

---

## 테이블 정의

### `ItemTemplateTable` — 아이템 공통 메타

> ItemTemplateTable은 모든 아이템 종류의 공통 메타(이름, 등급, 슬롯, 아이콘, 경제 규칙)를 저장하며, 인벤토리·드롭·UI가 아이템 종류를 특정할 때 단일 FK로 조회할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `ItemId` | INT | PK | |
| `ItemCode` | VARCHAR | DATA | 내부 코드명 |
| `ItemName` | VARCHAR | DATA | 표시 이름 |
| `ItemCategory` | ENUM | DATA | `WEAPON`, `ARMOR`, `ACCESSORY`, `CONSUMABLE`, `MATERIAL`, `SKILL_BOOK` |
| `Rarity` | ENUM | DATA | `NORMAL`, `UNCOMMON`, `RARE`, `EPIC` |
| `WeaponTypeTag` | ENUM | DATA | 무기 계열 아이템일 때만 사용 (`SWORD_SHIELD`, `GREAT_SWORD`, `STAFF`, NULL) |
| `EquipSlot` | ENUM | DATA | 장착 부위 (`WEAPON`, `CHEST`, `LEGS`, `HANDS`, `NECK`, `RING_L`, `RING_R`, NULL) |
| `BaseStatSetId` | INT | FK | → `BaseStatSetTable.BaseStatSetId` |
| `OptionPoolId` | INT | FK | → `OptionPoolTable.OptionPoolId` |
| `TradeLimitCount` | INT | DATA | 초기 거래 가능 횟수 (0 = 거래 불가) |
| `SellValue` | INT | DATA | NPC 판매 가치 (0 = 연습 보상 자동 배지) |
| `PracticeLootFlag` | BOOL | DATA | `정찰 원정` 전용 보상 여부 |
| `IconKey` | VARCHAR | META | 아이콘 리소스 키 |
| `UiStateTag` | VARCHAR | FK | → `ItemUiStateTable.UiStateTag` |

- PK: `ItemId`
- FK: `BaseStatSetId → BaseStatSetTable.BaseStatSetId`
- FK: `OptionPoolId → OptionPoolTable.OptionPoolId`
- FK: `UiStateTag → ItemUiStateTable.UiStateTag`

**무기 authoring 소유권 규칙:**
- `ItemTemplateTable`은 공통 아이템 메타(이름/등급/슬롯/경제)를 담당한다.
- 무기 전투 전용 메타(`WeaponPower`, `AttackSpeed`, `AnimationSetId` 등)는 `WeaponTable`이 담당한다.
- 두 테이블은 `ItemId` FK로 연결한다 (pending decision #6 기본안 B 반영).

---

### `BaseStatSetTable` — 기본 스탯 묶음

> BaseStatSetTable은 아이템 종류별 기본 스탯 집합을 저장하며, 장착 시 플레이어 스탯 계산에서 단일 조회로 기본 수치를 더할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `BaseStatSetId` | INT | PK | |
| `StatProfileType` | ENUM | DATA | `WEAPON`, `ARMOR`, `ACCESSORY`, `CONSUMABLE` |
| `AttackPower` | FLOAT | DATA | 공격력 |
| `WeaponPower` | FLOAT | DATA | 무기 공격력 |
| `Defense` | FLOAT | DATA | 방어력 |
| `MaxHP` | FLOAT | DATA | 최대 HP |
| `CriticalChance` | FLOAT | DATA | 치명타 확률 |
| `CriticalDamage` | FLOAT | DATA | 치명타 추가 피해 |
| `CooldownReduction` | FLOAT | DATA | 쿨다운 감소 |
| `ParryWindowBonus` | FLOAT | DATA | 패링 판정 보정 |
| `MoveSpeedBonus` | FLOAT | DATA | 이동 속도 보정 |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `BaseStatSetId`

---

### `OptionPoolTable` — 랜덤 옵션 풀

> OptionPoolTable은 아이템 등급별 랜덤 옵션 후보군을 저장하며, 드롭·재련 시 옵션 재굴림에서 풀 단위로 참조할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `OptionPoolId` | INT | PK | |
| `PoolName` | VARCHAR | DATA | 옵션 풀 이름 |
| `ItemCategory` | ENUM | DATA | 적용 아이템 분류 |
| `OptionTag` | VARCHAR | DATA | 옵션 종류 태그 |
| `MinValue` | FLOAT | DATA | 최솟값 |
| `MaxValue` | FLOAT | DATA | 최댓값 |
| `Weight` | FLOAT | DATA | 가중치 |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `OptionPoolId`

---

### `DropTable` — 드롭 가중치

> DropTable은 드롭 소스(몬스터/상자/기믹)별 아이템 드롭 가중치와 원정 모드 조건을 저장하며, 서버가 드롭 계산 시 소스 ID + RewardMode 조합으로 후보 행을 필터링할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `DropTableId` | INT | PK | |
| `SourceType` | ENUM | DATA | `NORMAL_MONSTER`, `ELITE`, `BOSS_CHEST`, `GIMMICK_BONUS` |
| `SourceId` | INT | DATA | 몬스터/상자/기믹 ID |
| `ItemId` | INT | FK | → `ItemTemplateTable.ItemId` |
| `BaseWeight` | FLOAT | DATA | 기본 가중치 |
| `MinCount` | INT | DATA | 최소 수량 |
| `MaxCount` | INT | DATA | 최대 수량 |
| `RewardMode` | ENUM | DATA | `PURIFICATION`, `SCOUTING`, `BOTH` |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `DropTableId`
- FK: `ItemId → ItemTemplateTable.ItemId`

---

### `DungeonRewardProfileTable` — 던전 보상 프로필

> DungeonRewardProfileTable은 던전 ID와 원정 모드 조합별 보상 정책(드롭 테이블, 전리품 bias, 상위 등급 허용 여부)을 저장하며, 서버가 던전 보상 계산 시 정책을 단일 행으로 조회할 수 있게 한다.

*(pending decision #7 기본안 B 반영 — 던전/원정 모드별 보상 프로필 분리)*

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `ProfileId` | INT | PK | |
| `DungeonId` | INT | DATA | 던전 ID |
| `RewardMode` | ENUM | DATA | `PURIFICATION`, `SCOUTING` |
| `DefaultDropTableId` | INT | FK | 기본 드롭 테이블 → `DropTable.DropTableId` |
| `BiasProfileId` | INT | FK | 전리품 선호 bias → `LootPreferenceBiasTable.BiasProfileId` |
| `AllowRareOrAbove` | BOOL | DATA | 희귀 이상 등급 드롭 허용 여부 |
| `BonusChestEnabled` | BOOL | DATA | 추가 보상 상자 활성 여부 |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `ProfileId`
- Unique: `DungeonId + RewardMode + BalanceVersion`
- FK: `DefaultDropTableId → DropTable.DropTableId`
- FK: `BiasProfileId → LootPreferenceBiasTable.BiasProfileId`

---

### `LootPreferenceBiasTable` — 전리품 선호 보정

> LootPreferenceBiasTable은 플레이어가 선택한 전리품 선호 무기에 따라 특정 무기 계열 드롭 가중치를 배율로 보정하며, DungeonRewardProfileTable이 참조하는 bias 집합이다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `BiasProfileId` | INT | PK | |
| `PreferredWeaponType` | ENUM | DATA | 플레이어가 선택한 선호 무기 타입 |
| `ItemCategory` | ENUM | DATA | bias 적용 아이템 분류 |
| `TargetWeaponType` | ENUM | DATA | 드롭 대상 무기 타입 |
| `WeightMultiplier` | FLOAT | DATA | 가중치 배율 |
| `Mode` | ENUM | DATA | `PURIFICATION`, `SCOUTING`, `BOTH` |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `BiasProfileId`

---

### `ReforgeCostTable` — 재련 비용

> ReforgeCostTable은 아이템 등급과 누적 재련 횟수에 따른 골드·재료 비용과 귀속 전환 규칙을 저장하며, 서버가 재련 요청 시 단일 조회로 비용 계산 근거를 가져올 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `Rarity` | ENUM | PK | 아이템 등급 |
| `AttemptCount` | INT | PK | 동일 아이템 누적 재련 횟수 |
| `GoldCost` | INT | DATA | 골드 비용 |
| `MaterialCostId` | INT | DATA | 추가 재화 ID (NULL = 골드만) |
| `BindOnReforge` | BOOL | DATA | 재련 시 계정 귀속 전환 여부 |

- PK: `Rarity + AttemptCount`

---

### `ItemUiStateTable` — UI 상태 배지

> ItemUiStateTable은 아이템의 귀속/거래 제한/연습 보상 UI 표현에 필요한 배지·색상·문구 키를 저장하며, 클라이언트가 아이템 툴팁·인벤토리 UI를 렌더링할 때 태그 하나로 일관된 표현을 로드할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `UiStateTag` | VARCHAR | PK | 상태 그룹 태그 |
| `BoundIconKey` | VARCHAR | META | 귀속 아이콘 리소스 키 |
| `TradeRemainFormat` | VARCHAR | META | 남은 거래 횟수 표시 형식 (예: `{n}회 거래 가능`) |
| `PracticeLootBadge` | VARCHAR | META | 연습 보상 배지 리소스 키 |
| `WarningColor` | VARCHAR | META | 경고 색상 코드 |
| `TooltipSuffix` | VARCHAR | META | 툴팁 후행 문구 |
| `LockReasonTag` | VARCHAR | META | 차단 사유 코드 (예: `BOUND`, `TRADE_ZERO`, `PRACTICE_ONLY`) |
| `TooltipReasonKey` | VARCHAR | META | 툴팁 차단 사유 문구 리소스 키 |

- PK: `UiStateTag`

---

## 테이블 연결 규칙

```
던전 입장 + 플레이어 전리품 선호 무기 선택
  → DungeonRewardProfileTable (DungeonId + RewardMode)
    → DropTable.DefaultDropTableId    (드롭 후보 행 필터링)
    → LootPreferenceBiasTable         (선호 무기 bias 적용)
      → DropTable.BaseWeight × WeightMultiplier  (최종 가중치 계산)

아이템 드롭 확정
  → ItemTemplateTable.ItemId
    → BaseStatSetTable (기본 스탯)
    → OptionPoolTable (랜덤 옵션 재굴림)
    → ItemUiStateTable (툴팁 UI 표현)

재련 요청
  → ReforgeCostTable (Rarity + AttemptCount)
    → GoldCost, MaterialCostId, BindOnReforge 확인
```

---

## 제약과 예외

- 실제 드롭 확률 수치는 플레이 로그와 파밍 속도 검증 후 조정한다.
- `SellValue = 0`인 아이템은 자동으로 `연습 보상` 배지를 노출하는 방향을 우선한다.
- `WeaponTable` (전투 전용 메타)와 이 문서의 `ItemTemplateTable` 연결 규칙은 `combat_data_tables.md`에 명시된다.
- 장비 슬롯 구성(`EquipSlot` enum 값)은 pending decision #5(v1 장비 슬롯 고정)에 따라 확정된다.
- 저장 구조와 패킷 구조는 서버 구현 문서에서 확정한다.
- 원정 모드 단일화(pending decision #4)가 결정되면 `DungeonRewardProfileTable`과 `DropTable.RewardMode` 구조가 단순화된다.

---

## 관련 문서

- [item.md](./item.md)
- [skill.md](./skill.md)
- [data-table-conventions.md](./data-table-conventions.md)
- [../outgame/economy.md](../outgame/economy.md)
- [../ingame/dungeon_progress.md](../ingame/dungeon_progress.md)
- [../ingame/combat_data_tables.md](../ingame/combat_data_tables.md)
