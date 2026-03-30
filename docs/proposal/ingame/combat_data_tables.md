# 전투 데이터 테이블 초안

## 목적

전투 스탯 데이터의 책임 범위와 테이블 연결 규칙만 정의한다. 실제 수치 행은 `CSV`, 반복 생성은 `generate_combat_tables.py`가 담당한다.

## 핵심 규칙

- 최대 유저 레벨은 `40`이다.
- 무기 강화 최대치는 `25`다.
- 유저 레벨 스탯은 무기 타입과 무관하다.
- 무기 장착과 강화로 바뀌는 값은 무기 스탯 테이블이 담당한다.
- 이 문서는 설명서이며, 실제 행 데이터는 포함하지 않는다.
- `BalanceVersion`으로 CSV 세트를 병행 관리한다.

## 생성 파일

- [player_stat_table.csv](./data/player_stat_table.csv)
- [user_level_stat_link_table.csv](./data/user_level_stat_link_table.csv)
- [weapon_stat_table.csv](./data/weapon_stat_table.csv)
- [weapon_level_stat_link_table.csv](./data/weapon_level_stat_link_table.csv)
- [weapon_table.csv](./data/weapon_table.csv)
- [generate_combat_tables.py](../../../tools/generate_combat_tables.py)

## 생성 규칙

```text
Player.Level -> UserLevelStatLinkTable.Level -> PlayerStatTableId
Weapon.Type + Weapon.EnhanceLevel -> WeaponLevelStatLinkTable -> WeaponStatTableId
WeaponTable.BaseWeaponStatTableId -> WeaponStatTableId
```

- `md`는 계약과 연결 규칙만 담는다.
- `csv`는 사람이 검토하는 원본 데이터다.
- 생성 스크립트는 규칙적으로 증가하는 행과 FK 연결을 검증한다.

## 데이터 책임

### `PlayerStatTable`

유저 레벨 성장에 따라 변하는 공통 스탯 스냅샷이다.

- 담당 값: `MaxHP`, `AttackPower`, `Defense`, `CriticalChance`, `CriticalDamage`, `CooldownReduction`, `DamageReduction`, `MoveSpeedBonus`
- 제외 값: `WeaponPower`, `AttackSpeed`, `CastSpeed`, `ResourceMax`, `IdentityGaugeMax`

### `UserLevelStatLinkTable`

유저 레벨을 `PlayerStatTableId`에 연결하는 링크 테이블이다.

- 역할: `Level -> PlayerStatTableId`
- 목적: 레벨별 스탯 스냅샷을 안정적으로 참조하기 위함

### `WeaponStatTable`

무기 타입과 강화 단계가 제공하는 전투 스탯 스냅샷이다.

- 담당 값: `WeaponPower`, `BaseMoveSpeed`, `AttackSpeed`, `CastSpeed`, `IdentityGaugeMax`, `IdentityGaugeGain`, `ResourceType`, `ResourceMax`, `ResourceRegen`, `StaggerPower`, `ThreatGen`, `ParryWindowBonus`
- 목적: 무기 강화에 따른 전투 성능 변화를 저장하기 위함

### `WeaponLevelStatLinkTable`

무기 종류와 강화 레벨을 `WeaponStatTableId`에 연결하는 링크 테이블이다.

- 역할: `WeaponType + EnhanceLevel -> WeaponStatTableId`
- 목적: 무기 강화 스탯을 직접 참조하기 위함

### `WeaponTable`

무기 템플릿과 메타데이터를 저장하는 테이블이다.

- 담당 값: `WeaponCode`, `WeaponName`, `WeaponType`, `BaseWeaponStatTableId`, `MaxEnhanceLevel`, `SkillSetId`, `IdentitySkillId`, `SmartDropTag`, `OptionPoolId`, `BaseDurabilityMax`, `RepairCostRate`, `EquipLevelMin`, `TradeLimitCount`, `AnimationSetId`, `IconKey`, `ModelKey`, `DisplayOrder`, `HandType`, `CombatRoleTag`, `RangeProfile`, `ResourceType`
- 목적: 전투 수치와 무기 메타를 분리해 참조하기 위함

## PK / FK 규칙

- `PlayerStatTable`
  - PK: `PlayerStatTableId`
  - Unique: `Level + BalanceVersion`
- `UserLevelStatLinkTable`
  - PK: `Level + BalanceVersion`
  - FK: `PlayerStatTableId -> PlayerStatTable.PlayerStatTableId`
  - Check: `Level BETWEEN 1 AND 40`
- `WeaponStatTable`
  - PK: `WeaponStatTableId`
  - Unique: `WeaponType + EnhanceLevel + BalanceVersion`
- `WeaponLevelStatLinkTable`
  - PK: `WeaponType + EnhanceLevel + BalanceVersion`
  - FK: `WeaponStatTableId -> WeaponStatTable.WeaponStatTableId`
  - Check: `EnhanceLevel BETWEEN 1 AND 25`
- `WeaponTable`
  - PK: `WeaponId`
  - FK: `BaseWeaponStatTableId -> WeaponStatTable.WeaponStatTableId`
  - FK: `SkillSetId -> SkillSetTable.SkillSetId`
  - FK: `IdentitySkillId -> SkillTable.SkillId`

## 주의할 점

- `PlayerStatTable`는 유저 기본 성장만 담당하고, 무기 성장 값은 넣지 않는다.
- `WeaponStatTable`는 전투 수치만 담당하고, 템플릿 메타는 `WeaponTable`에 둔다.
- `WeaponStatTableId`는 안정적인 규칙으로 생성하고, 스크립트가 임의 재배치하지 않게 유지한다.
- 강화 단계가 실제 장비 상태라면 구현 단계에서 별도 소유 테이블이 필요하다.

## 관련 문서

- [combat_stats.md](./combat_stats.md)
- [combat_formulas.md](./combat_formulas.md)
- [../common/item.md](../common/item.md)
- [../common/skill.md](../common/skill.md)
