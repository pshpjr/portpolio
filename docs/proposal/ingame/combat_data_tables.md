# 전투 데이터 테이블 초안

## 목적

전투 스탯을 실제 데이터 파일 형태로 고정하고, 유저 레벨 테이블과 무기 강화 테이블이 어떤 스탯 인덱스를 참조하는지 명확히 정의한다.

## 핵심 규칙

- 최대 유저 레벨은 `40`이다.
- 무기 레벨은 무기 강화 단계로 해석한다.
- 무기 강화 최대치는 `25`다.
- 유저는 어떤 무기든 장착할 수 있으므로, `유저 레벨 스탯`은 무기 타입에 따라 바뀌지 않는다.
- 무기 장착과 강화로 바뀌는 값은 `WeaponStatTable`에서 관리한다.
- 실제 수치 원본은 `CSV`에 두고, 이 문서는 `스키마/PK-FK/생성 규칙`만 유지한다.
- 반복 수치는 루트 생성 스크립트로 재생성한다.

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
Weapon.Id + Weapon.EnhanceLevel -> WeaponLevelStatLinkTable -> WeaponStatTableId
WeaponTable.WeaponStatTableId + EnhanceLevel -> WeaponStatTable 복합 키 조회
```

- `md`는 계약서 역할만 한다.
- `csv`는 사람이 검토하는 원본 데이터다.
- 생성 스크립트는 규칙적으로 증가하는 행을 다시 만들고, 기본 단조 증가/FK 연결을 검증한다.
- `BalanceVersion`은 모든 CSV에 넣어 같은 구조에서 버전 병행이 가능하게 둔다.

## 권장 테이블 구조

### 1. `PlayerStatTable`

유저 레벨 성장에 따라 변하는 공통 스탯 스냅샷 테이블이다.

| 컬럼 | 타입 | 설명 |
|---|---|---|
| `PlayerStatTableId` | PK int | 유저 스탯 인덱스 |
| `Level` | int | 유저 레벨. `1 ~ 40` |
| `MaxHP` | int | 공통 최대 체력 |
| `AttackPower` | int | 공통 공격 성장값 |
| `Defense` | int | 공통 방어 성장값 |
| `CriticalChance` | float | 공통 치명타 확률 |
| `CriticalDamage` | float | 공통 치명타 추가 피해 |
| `CooldownReduction` | float | 공통 쿨다운 감소 |
| `DamageReduction` | float | 공통 최종 피해 감소 |
| `MoveSpeedBonus` | float | 공통 이동 속도 보정 |
| `BalanceVersion` | int | 밸런스 버전 |

- 이 테이블은 유저 자체 성장만 담는다.
- `WeaponPower`, `AttackSpeed`, `CastSpeed`, `ResourceMax`, `IdentityGaugeMax`는 넣지 않는다.

### 2. `UserLevelStatLinkTable`

유저 레벨을 `PlayerStatTableId`에 연결하는 링크 테이블이다.

| 컬럼 | 타입 | 설명 |
|---|---|---|
| `Level` | PK int | 유저 레벨 |
| `PlayerStatTableId` | FK int | `PlayerStatTable.PlayerStatTableId` |
| `BalanceVersion` | PK int | 밸런스 버전 |

- 사용자가 원한 `레벨을 키로 스탯 테이블 인덱스를 링크` 구조는 이 테이블에서 해결한다.
- v1에서는 `Level`과 `PlayerStatTable.Level`이 1:1이지만, 향후 버전 병행 때문에 링크 테이블을 유지한다.

### 3. `WeaponStatTable`

무기 타입과 강화 단계가 제공하는 전투 스탯 스냅샷 테이블이다.

| 컬럼 | 타입 | 설명 |
|---|---|---|
| `WeaponStatTableId` | PK int | 무기 스탯 묶음 인덱스 |
| `EnhanceLevel` | PK int | 무기 강화 레벨. `1 ~ 25` |
| `WeaponPower` | int | 무기 공격 성장값 |
| `BaseMoveSpeed` | float | 무기 장착 기준 기본 이동 속도 |
| `AttackSpeed` | float | 무기 공격 속도 보정 |
| `CastSpeed` | float | 무기 시전 속도 보정 |
| `IdentityGaugeMax` | int | 무기 아이덴티티 최대 게이지 |
| `IdentityGaugeGain` | float | 무기 아이덴티티 획득량 보정 |
| `ResourceType` | enum | 자원 타입 |
| `ResourceMax` | int | 무기 자원 최대치 |
| `ResourceRegen` | int | 무기 자원 회복량 |
| `StaggerPower` | int | 무기 무력화 성능 |
| `ThreatGen` | float | 무기 위협도 생성 보정 |
| `ParryWindowBonus` | float | 무기 패링 보정 |
| `BalanceVersion` | int | 밸런스 버전 |

- `WeaponStatTableId + EnhanceLevel`이 실제 스탯 행의 복합 키다.
- 강화 단계별로 주로 `WeaponPower`, `StaggerPower`, `ResourceMax`, `ResourceRegen`을 조정한다.
- `BaseMoveSpeed`, `AttackSpeed`, `CastSpeed`, `ThreatGen`, `ParryWindowBonus` 같은 무기 정체성 값은 v1에서 고정 유지한다.

### 4. `WeaponLevelStatLinkTable`

무기 인스턴스가 가진 `WeaponId + 강화 레벨`을 `WeaponStatTableId`에 연결하는 조회용 링크 테이블이다.

| 컬럼 | 타입 | 설명 |
|---|---|---|
| `WeaponId` | PK int | 무기 템플릿 인덱스 |
| `EnhanceLevel` | PK int | 무기 강화 레벨. `1 ~ 25` |
| `WeaponStatTableId` | FK int | `WeaponStatTable.WeaponStatTableId` |
| `BalanceVersion` | PK int | 밸런스 버전 |

- 이 테이블은 `무기 ID + 강화 단계` 기준 조회 경로를 명시하기 위한 구조다.
- 무기 강화는 캐릭터 레벨과 별도 축이므로 `UserLevelStatLinkTable`과 분리 유지한다.

### 5. `WeaponTable`

무기 템플릿과 무기 종류 메타데이터를 저장하는 테이블이다.

| 컬럼 | 타입 | 설명 |
|---|---|---|
| `WeaponId` | PK int | 무기 템플릿 인덱스 |
| `WeaponCode` | UK string | 데이터/툴링용 코드 |
| `WeaponName` | string | 표시 이름 |
| `WeaponType` | enum | 무기 분류 |
| `WeaponStatTableId` | FK int | 무기 스탯 묶음 인덱스 |
| `MaxEnhanceLevel` | int | 무기 강화 최대치 |
| `SkillSetId` | FK | 무기 공용 스킬 세트 |
| `IdentitySkillId` | FK | 아이덴티티 스킬 |
| `SmartDropTag` | string | 스마트 드롭 태그 |
| `OptionPoolId` | FK | 랜덤 옵션 풀 |
| `BaseDurabilityMax` | int | 기본 내구도 |
| `RepairCostRate` | float | 수리 비용 계수 |
| `EquipLevelMin` | int | 장착 최소 레벨 |
| `TradeLimitCount` | int | 거래 가능 횟수 |
| `AnimationSetId` | FK | 애니메이션 세트 |
| `IconKey` | string | UI 아이콘 키 |
| `ModelKey` | string | 모델 키 |
| `DisplayOrder` | int | 정렬 순서 |
| `HandType` | enum | 장착 방식. `ONE_HAND_SHIELD`, `TWO_HAND`, `CHANNELING` |
| `CombatRoleTag` | enum | 전투 역할 태그. `GUARD`, `BURST`, `CASTER` |
| `RangeProfile` | enum | 사거리 프로필. `MELEE`, `RANGED` |
| `BalanceVersion` | int | 밸런스 버전 |

- `WeaponTable`은 템플릿과 메타데이터가 본체다.
- 실제 전투 계산의 기준 수치는 `WeaponStatTable`이 권한을 가진다.

## PK / FK 규칙

- `PlayerStatTable`
  - PK: `PlayerStatTableId`
  - Unique: `Level + BalanceVersion`
- `UserLevelStatLinkTable`
  - PK: `Level + BalanceVersion`
  - FK: `PlayerStatTableId -> PlayerStatTable.PlayerStatTableId`
  - Check: `Level BETWEEN 1 AND 40`
- `WeaponStatTable`
  - PK: `WeaponStatTableId + EnhanceLevel + BalanceVersion`
- `WeaponLevelStatLinkTable`
  - PK: `WeaponId + EnhanceLevel + BalanceVersion`
  - FK: `WeaponId -> WeaponTable.WeaponId`
  - FK: `WeaponStatTableId -> WeaponStatTable.WeaponStatTableId`
  - Check: `EnhanceLevel BETWEEN 1 AND 25`
- `WeaponTable`
  - PK: `WeaponId`
  - FK: `WeaponStatTableId -> WeaponStatTable.WeaponStatTableId`
  - FK: `SkillSetId -> SkillSetTable.SkillSetId`
  - FK: `IdentitySkillId -> SkillTable.SkillId`

## 수치 생성 기준

### 플레이어 성장

```text
PlayerStatTableId = 1000 + Level
MaxHP = 1000 + (Level - 1) * 80
AttackPower = 80 + (Level - 1) * 6
Defense = 100 + (Level - 1) * 9
```

- `CriticalChance`, `CriticalDamage`, `CooldownReduction`, `DamageReduction`, `MoveSpeedBonus`는 v1에서 고정값으로 생성한다.

### 무기 강화 성장

검방:

```text
WeaponStatTableId = 200
WeaponPower = 60 + (EnhanceLevel - 1) * 4
StaggerPower = 110 + (EnhanceLevel - 1) * 2
ResourceMax = 100 + floor((EnhanceLevel - 1) / 5) * 2
ResourceRegen = 6 + floor((EnhanceLevel - 1) / 10)
```

대검:

```text
WeaponStatTableId = 210
WeaponPower = 85 + (EnhanceLevel - 1) * 5
StaggerPower = 135 + (EnhanceLevel - 1) * 3
ResourceMax = 100 + floor((EnhanceLevel - 1) / 5) * 2
ResourceRegen = 5 + floor((EnhanceLevel - 1) / 10)
```

지팡이:

```text
WeaponStatTableId = 220
WeaponPower = 110 + (EnhanceLevel - 1) * 5
StaggerPower = 90 + (EnhanceLevel - 1) * 2
ResourceMax = 140 + floor((EnhanceLevel - 1) / 5) * 3
ResourceRegen = 8 + floor((EnhanceLevel - 1) / 10)
```

- 속도 계열과 패링 보정은 강화가 아니라 무기 정체성에 가깝기 때문에 고정 유지한다.
- v2에서는 필요하면 `구간 오버라이드`나 `무기별 예외 값`을 스크립트 입력으로 추가한다.

## 왜 이 구조가 더 맞는가

- 유저 기본 성장과 무기 장착/강화 효과를 분리해서, 장비 교체가 유저 레벨 테이블을 흔들지 않는다.
- 큰 표를 `md`에 직접 붙이지 않아 문서 컨텍스트를 아낄 수 있다.
- `CSV + 생성 스크립트` 구조라 규칙적 증가 테이블을 빠르게 재생성할 수 있다.
- `WeaponTable`은 무기 종류 메타와 참조 관계만 유지하므로 역할이 더 명확하다.
- 실제 계산 권한은 `WeaponStatTable`에 남겨 두어 중복 책임이 과해지지 않는다.

## 주의할 점

- `AttackPower`와 `WeaponPower`를 같은 성장 축으로 합치지 않는다.
- `ResourceType`는 현재 `WeaponStatTable`을 단일 소스로 사용한다.
- 강화 단계가 실제 장비 상태라면 구현 단계에서는 `WeaponInstance` 또는 동등한 소유 테이블이 따로 필요하다.
- `WeaponStatTableId`는 안정적인 규칙으로 생성하고, 스크립트가 임의 재배치하지 않게 유지한다.

## 관련 문서

- [combat_stats.md](./combat_stats.md)
- [combat_formulas.md](./combat_formulas.md)
- [../common/item.md](../common/item.md)
- [../common/skill.md](../common/skill.md)
