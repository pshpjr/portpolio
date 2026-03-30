# 전투 데이터 테이블 초안

## 목적

전투 스탯 데이터의 책임 범위와 테이블 연결 규칙만 정의한다. 실제 행 데이터는 `data.json`, 필드 계약과 생성 메타는 `schema.json`, 서버/언리얼 코드 생성은 `combat_data_gen.py`가 담당한다.

## 핵심 규칙

- 최대 유저 레벨은 `40`이다.
- 무기 강화 최대치는 `25`다.
- 유저 레벨 스탯은 무기 타입과 무관하다.
- 무기 장착과 강화로 바뀌는 값은 무기 스탯 테이블이 담당한다.
- 이 문서는 설명서이며, 실제 행 데이터는 포함하지 않는다.
- `BalanceVersion`으로 데이터 세트를 병행 관리한다.

## 소스 파일

- [player_stat_table.schema.json](./data/player_stat_table.schema.json)
- [player_stat_table.data.json](./data/player_stat_table.data.json)
- [user_level_stat_link_table.schema.json](./data/user_level_stat_link_table.schema.json)
- [user_level_stat_link_table.data.json](./data/user_level_stat_link_table.data.json)
- [weapon_stat_table.schema.json](./data/weapon_stat_table.schema.json)
- [weapon_stat_table.data.json](./data/weapon_stat_table.data.json)
- [weapon_level_stat_link_table.schema.json](./data/weapon_level_stat_link_table.schema.json)
- [weapon_level_stat_link_table.data.json](./data/weapon_level_stat_link_table.data.json)
- [weapon_table.schema.json](./data/weapon_table.schema.json)
- [weapon_table.data.json](./data/weapon_table.data.json)
- [combat_data_gen.py](../../../server/tools/codegen/combat_data_gen.py)

## 편집 규칙

- `schema.json`은 필드 이름, 타입, enum, key, 런타임 메타를 정의한다.
- `data.json`은 실제 행 데이터만 담는다.
- 에이전트는 새 컬럼 추가 시 `schema.json`을 먼저 바꾸고, 그다음 `data.json`에 값을 채운다.
- 서버와 언리얼 로딩 타입은 생성 코드만 수정 대상으로 보고, 수동 편집하지 않는다.

## 생성 규칙

```text
Player.Level -> UserLevelStatLinkTable.Level -> PlayerStatTableId
Weapon.Type + Weapon.EnhanceLevel -> WeaponLevelStatLinkTable -> WeaponStatTableId
WeaponTable.BaseWeaponStatTableId -> WeaponStatTableId
```

- `md`는 계약과 연결 규칙만 담는다.
- `schema.json`은 DTO/USTRUCT 생성 계약이다.
- `data.json`은 에이전트가 수정하는 원본 데이터다.
- 생성 스크립트는 필수 필드와 테이블 메타를 검증한다.

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

- 담당 값: `WeaponCode`, `WeaponName`, `WeaponType`, `BaseWeaponStatTableId`, `MaxEnhanceLevel`, `SkillSetId`, `IdentitySkillId`, `SmartDropTag`, `OptionPoolId`, `BaseDurabilityMax`, `RepairCostRate`, `EquipLevelMin`, `TradeLimitCount`, `AnimationSetId`, `IconKey`, `ModelKey`, `DisplayOrder`, `HandType`, `CombatRoleTag`, `RangeProfile`, `BalanceVersion`
- 목적: 전투 수치와 무기 메타를 분리해 참조하기 위함

## v1 장비 데이터 책임

### 방어구/장신구 템플릿

- v1에서는 무기처럼 별도 강화 단계 테이블을 크게 벌리지 않고, `ItemTemplate + FixedStatSet + OptionPool` 조합으로 시작한다.
- 방어구 템플릿은 `SlotType`, `EquipLevelMin`, `BaseDefense`, `BaseHP`, `OptionPoolId`, `TradeLimitCount`를 중심으로 관리한다.
- 장신구 템플릿은 `SlotType`, `EquipLevelMin`, `BaseCritChance`, `BaseCritDamage`, `BaseCooldownReduction`, `OptionPoolId`, `TradeLimitCount`를 중심으로 관리한다.
- 강화 단계가 늘어나거나 등급별 반복 행이 많아지면 후속 단계에서 `EquipStatTable` 계열로 분리할 수 있다.

### 옵션 풀

- `OptionPoolId`는 장비군별로 분리한다.
- 무기 옵션 풀은 공격/무력화/아이덴티티 계열, 방어구 옵션 풀은 생존 계열, 장신구 옵션 풀은 운용 보조 계열을 우선한다.
- 재련은 `OptionPoolId` 안에서 단일 옵션 재굴림을 수행하는 구조를 전제로 한다.

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
- 방어구/장신구는 v1에서 무기와 같은 복잡한 강화 테이블을 강제하지 않는다.
- `WeaponStatTableId`는 안정적인 규칙으로 생성하고, 스크립트가 임의 재배치하지 않게 유지한다.
- 강화 단계가 실제 장비 상태라면 구현 단계에서 별도 소유 테이블이 필요하다.

## 관련 문서

- [combat_stats.md](./combat_stats.md)
- [combat_formulas.md](./combat_formulas.md)
- [../common/item.md](../common/item.md)
- [../common/skill.md](../common/skill.md)
