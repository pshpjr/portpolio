# 아이템 / 드롭 데이터 테이블 구조

## 목적

아이템, 드롭, 전리품 선호, 재련 비용 authoring에 필요한 테이블 구조를 정의한다. 실제 값은 JSON/CSV로 관리하고, 이 문서는 컬럼 계약과 책임만 다룬다.

## 핵심 규칙

- 아이템 템플릿과 드롭 규칙은 분리한다.
- 전리품 선호 무기 보정은 별도 bias 테이블에서 관리한다.
- 경제 표기는 플레이어가 읽는 상태(`거래 가능`, `계정 귀속`, `연습 보상`)와 런타임 상태를 같이 담을 수 있어야 한다.

## 테이블 구조

### `ItemTemplateTable`

| 컬럼 | 설명 |
|---|---|
| `ItemId` | PK |
| `ItemCode` | 내부 코드 |
| `ItemName` | 표시 이름 |
| `ItemCategory` | `WEAPON`, `ARMOR`, `ACCESSORY`, `CONSUMABLE`, `MATERIAL`, `SKILL_BOOK` |
| `Rarity` | `NORMAL`, `UNCOMMON`, `RARE`, `EPIC` |
| `WeaponTypeTag` | 무기 관련 아이템일 때만 사용 |
| `EquipSlot` | 장착 부위 |
| `BaseStatSetId` | 기본 스탯 묶음 참조 |
| `OptionPoolId` | 옵션 풀 참조 |
| `TradeLimitCount` | 초기 거래 가능 횟수 |
| `SellValue` | NPC 판매 가치 |
| `PracticeLootFlag` | `정찰 원정` 전용 보상 여부 |
| `IconKey` | 아이콘 리소스 키 |
| `UiStateTag` | UI 표기 그룹 |

### `DropTable`

| 컬럼 | 설명 |
|---|---|
| `DropTableId` | PK |
| `SourceType` | `NORMAL_MONSTER`, `ELITE`, `BOSS_CHEST`, `GIMMICK_BONUS` |
| `SourceId` | 몬스터/상자/기믹 ID |
| `ItemId` | FK -> `ItemTemplateTable` |
| `BaseWeight` | 기본 가중치 |
| `MinCount` | 최소 수량 |
| `MaxCount` | 최대 수량 |
| `RewardMode` | `PURIFICATION`, `SCOUTING`, `BOTH` |
| `BalanceVersion` | 밸런스 버전 |

### `LootPreferenceBiasTable`

| 컬럼 | 설명 |
|---|---|
| `BiasProfileId` | PK |
| `PreferredWeaponType` | 전리품 선호 무기 |
| `ItemCategory` | 어떤 카테고리에 bias를 줄지 |
| `TargetWeaponType` | 드롭 대상 무기 타입 |
| `WeightMultiplier` | 가중치 배율 |
| `Mode` | `PURIFICATION`, `SCOUTING`, `BOTH` |

### `ReforgeCostTable`

| 컬럼 | 설명 |
|---|---|
| `Rarity` | 아이템 등급 |
| `AttemptCount` | 동일 아이템 누적 재련 횟수 |
| `GoldCost` | 골드 비용 |
| `MaterialCostId` | 추가 재화 ID |
| `BindOnReforge` | 재련 시 계정 귀속 여부 |

### `ItemUiStateTable`

| 컬럼 | 설명 |
|---|---|
| `UiStateTag` | PK |
| `BoundIconKey` | 귀속 아이콘 |
| `TradeRemainFormat` | 남은 거래 횟수 표시 형식 |
| `PracticeLootBadge` | 연습 보상 배지 |
| `WarningColor` | 경고 색상 |
| `TooltipSuffix` | 툴팁 후행 문구 |

## 제약과 예외

- 실제 드롭 확률 수치는 플레이 로그와 파밍 속도 검증 후 조정한다.
- `SellValue`가 0인 아이템은 자동으로 `연습 보상` 배지를 노출하는 방향을 우선한다.
- 저장 구조와 패킷 구조는 구현 문서에서 확정한다.

## 관련 문서

- [item.md](./item.md)
- [skill.md](./skill.md)
- [../outgame/economy.md](../outgame/economy.md)
- [../ingame/dungeon_progress.md](../ingame/dungeon_progress.md)
