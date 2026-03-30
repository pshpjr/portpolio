# 전투 스탯 초안

## 목적

전투 공식, 밸런스 엑셀, 서버 로그 검증에서 공통으로 사용할 `v1 전투 스키마`를 먼저 고정한다.

## 핵심 규칙

- 이 문서는 포트폴리오 기준 1차 구현에 필요한 전투 스탯만 우선 고정한다.
- 플레이어 공통 전투 스탯과, 스킬/보스 데이터에서 관리할 값을 분리한다.
- 수치 밸런스는 후속 엑셀에서 조정할 수 있지만 필드 이름과 책임 경계는 여기서 먼저 닫는다.
- 패링, 무력화, 무기 정체성을 1차 코어로 보고 속성/위협도/상태 이상은 단순형 또는 확장형으로 관리한다.

## 기본 단위와 범위

- 캐릭터 레벨 기준 범위는 `1 ~ 40`을 사용한다.
- 1차 구현 기준 기본 플레이어는 레벨 1에서 방어구 미장착 상태로 시작한다.
- 기본 전투 스탯은 정수값을 사용하고, 퍼센트형 보정은 소수 둘째 자리까지 허용한다.
- 서버 내부 계산은 부동소수점으로 처리하되, 최종 피해와 게이지 감소량은 소수 첫째 자리에서 반올림 후 정수로 확정한다.
- 모든 퍼센트형 증감 스탯은 내부적으로 `0.15 = 15%` 형식을 사용한다.

## v1 플레이어 기본값

| 스탯 | 값 |
|---|---:|
| `MaxHP` | 1000 |
| `AttackPower` | 80 |
| `Defense` | 100 |
| `CriticalChance` | 0.05 |
| `CriticalDamage` | 0.50 |
| `CooldownReduction` | 0.00 |
| `DamageReduction` | 0.00 |
| `MoveSpeedBonus` | 0.00 |

- 위 표는 "무기 미반영, 노옵션, 노버프, 레벨 1 유저 공통값"이다.
- 어떤 무기를 들더라도 유저 레벨 테이블은 유지되고, 무기 장착으로 최종값이 보정된다.

## 레벨 성장 규칙

### 플레이어 레벨업 당 증가량

| 스탯 | 증가량 |
|---|---:|
| `MaxHP` | +80 |
| `AttackPower` | +6 |
| `Defense` | +9 |

- 레벨 성장으로 `CriticalChance`, `CriticalDamage`, `CooldownReduction`, `DamageReduction`은 직접 증가하지 않는다.
- 이런 보조 스탯은 무기, 장비, 룬, 스킬 변형, 버프로만 확보한다.

## v1 무기 기본값

| 스탯 | 검방 | 대검 | 지팡이 |
|---|---:|---:|---:|
| `WeaponPower` | 60 | 85 | 110 |
| `BaseMoveSpeed` | 4.2 | 4.0 | 4.3 |
| `AttackSpeed` | 0.00 | -0.05 | 0.05 |
| `CastSpeed` | -0.05 | -0.10 | 0.15 |
| `IdentityGaugeMax` | 1000 | 1000 | 1000 |
| `IdentityGaugeGain` | 0.00 | 0.00 | 0.00 |
| `ResourceMax` | 100 | 100 | 140 |
| `ResourceRegen` | 6 | 5 | 8 |
| `StaggerPower` | 110 | 135 | 90 |
| `ThreatGen` | 0.30 | 0.05 | -0.10 |
| `ParryWindowBonus` | 0.04 | 0.00 | -0.02 |

- 위 표는 "해당 무기 장착 시 적용되는 기본 무기 스탯"이다.
- `BaseMoveSpeed`, `AttackSpeed`, `CastSpeed`, `ThreatGen`, `ParryWindowBonus`는 무기 장착으로 바뀌는 값이다.

## v1 상한선

| 스탯 | 상한 |
|---|---:|
| `CriticalChance` | 0.40 |
| `CriticalDamage` 추가분 | 1.00 |
| `CooldownReduction` | 0.35 |
| `AttackSpeed` | 0.40 |
| `CastSpeed` | 0.50 |
| `MoveSpeedBonus` | 0.25 |
| `DamageReduction` | 0.60 |
| `ThreatGen` | 2.00 |
| `ParryWindowBonus` | 0.08 |

- 상한선은 장비와 버프를 모두 포함한 최종값 기준이다.
- 서버는 상한 적용 후 공식 계산에 들어간다.

## v1 필수 전투 스키마

### 1. 플레이어/몬스터 공통 기본 스탯

- `MaxHP`: 체력 최대치.
- `AttackPower`: 캐릭터 성장 중심 공격 값.
- `Defense`: 상시 방어 감쇠용 값.
- `CriticalChance`: 치명타 발생 확률.
- `CriticalDamage`: 치명타 성공 시 추가 피해 배율.
- `CooldownReduction`: 재사용 대기시간 단축.
- `DamageReduction`: 최종 단계 피해 감소 보정.
- `MoveSpeedBonus`: 기본 이동 속도에 곱해지는 공통 이동 속도 보정.

### 2. 무기 공통 기본 스탯

- `WeaponPower`: 무기 성장 중심 공격 값.
- `BaseMoveSpeed`: 기본 이동 속도.
- `ResourceMax`: 무기별 자원 최대치.
- `ResourceRegen`: 전투 중 자원 회복 속도.
- `IdentityGaugeMax`: 아이덴티티 발동에 필요한 최대 게이지.
- `IdentityGaugeGain`: 적중 시 아이덴티티 획득량 보정.
- `StaggerPower`: 무력화 게이지 감소 성능.
- `StaggerResistance`: 무력화 저항 값.
- `ThreatGen`: 피해/도발 기반 위협도 보정.
- `ParryWindowBonus`: 패링 가능 시간 보정.

### 3. v1 보조 스탯

- `AttackSpeed`: 기본 공격 및 일부 스킬 실행 속도 보정.
- `CastSpeed`: 시전형 스킬 준비 속도 보정.
- `ElementAttack_X`: 속성 추가 피해용 공격 값.
- `ElementResist_X`: 속성 저항 값.

### 4. 스킬 데이터로 내려야 하는 값

- `SkillCoeff`
- `SkillStaggerCoeff`
- `SkillGaugeCoeff`
- `SkillThreatBase`
- `IdentityCost`
- `BaseCooldown`
- `BaseResourceCost`
- `StatusBuildUpPerHit`

- 위 값들은 캐릭터 기본 스탯이 아니라 스킬 정의 데이터 또는 밸런스 테이블에서 관리한다.

### 5. 보스/몬스터 데이터로 내려야 하는 값

- `StaggerGaugeMax`
- `DownDuration`
- `RecoveryDuration`
- `AggroChangeThreshold`
- `PatternWeight`
- `DetectionRange`
- `ForcedTargetingFlag`
- `StatusImmunityFlag`

- 위 값들은 보스 패턴과 AI 문서가 책임지는 데이터다.

## 확장 후보 스탯

### 1. 상태 이상/제어 확장

- `StatusBuildUp`
- `StatusDuration`
- `StatusAmplify`
- `StatusResistance`
- `ControlPower`
- `ControlResistance`

### 2. 포지셔닝/특수 피해 확장

- `WeakPointPower`
- `PositionalBonus`
- `CounterPower`
- `ThreatDecay`

### 3. 장비/경제 연동 확장

- `Durability`
- `DurabilityLossOnDeath`

- `RepairCostRate`, `OptionSlotCount`, `SmartDropWeight`, `TradeLimitCount`는 전투 스탯이 아니라 아이템/경제 문서에서 관리한다.

## 장비 옵션 수치 범위

### 무기 옵션

| 옵션 | 레어 | 에픽 |
|---|---:|---:|
| `AttackPower` | +12 ~ +24 | +24 ~ +42 |
| `WeaponPower` | +10 ~ +20 | +20 ~ +36 |
| `CriticalChance` | +0.02 ~ +0.05 | +0.05 ~ +0.09 |
| `CriticalDamage` | +0.08 ~ +0.16 | +0.16 ~ +0.28 |
| `StaggerPower` | +12 ~ +30 | +30 ~ +55 |
| `ElementAttack_X` | +15 ~ +35 | +35 ~ +65 |

### 방어구/장신구 옵션

| 옵션 | 레어 | 에픽 |
|---|---:|---:|
| `MaxHP` | +80 ~ +180 | +180 ~ +320 |
| `Defense` | +15 ~ +35 | +35 ~ +60 |
| `DamageReduction` | +0.01 ~ +0.03 | +0.03 ~ +0.05 |
| `ParryWindowBonus` | +0.01 ~ +0.03 | +0.03 ~ +0.05 |
| `CooldownReduction` | +0.01 ~ +0.03 | +0.03 ~ +0.05 |
| `ThreatGen` | +0.10 ~ +0.25 | +0.25 ~ +0.45 |
| `MoveSpeedBonus` | +0.01 ~ +0.03 | +0.03 ~ +0.05 |

## 구현 우선 필드

- Phase 2까지 반드시 구현할 필드:
  - `MaxHP`, `AttackPower`, `WeaponPower`, `Defense`, `BaseMoveSpeed`
  - `IdentityGaugeMax`, `IdentityGaugeGain`, `ResourceMax`, `ResourceRegen`
  - `StaggerPower`, `StaggerResistance`
  - `CriticalChance`, `CriticalDamage`
  - `ParryWindowBonus`
- Phase 3 이후 확장 필드:
  - `ThreatGen`, `ElementAttack_X`, `ElementResist_X`
  - `CooldownReduction`, `AttackSpeed`, `CastSpeed`, `MoveSpeedBonus`
  - `StatusBuildUp`, `StatusDuration`, `StatusAmplify`
  - `ControlPower`, `ControlResistance`, `ThreatDecay`

## 제약과 예외

- `AttackPower`와 `WeaponPower`는 1차 구현에서는 분리 유지한다.
- 유저 레벨 성장과 무기 장착 스탯은 분리 유지한다.
- `BaseMoveSpeed`와 `MoveSpeedBonus`는 분리 유지한다.
- `Defense`와 `DamageReduction`도 분리 유지한다. 전자는 상시 감쇠용, 후자는 특수 효과용이다.
- 상태 이상 종류 자체는 아직 별도 문서가 없으므로 1차 전투 코어에서는 필수 스키마로 고정하지 않는다.
- 수치 범위는 1차 구현 기준이며, 실제 던전 TTK와 로그를 보고 조정할 수 있다.

## 관련 문서

- [combat.md](./combat.md)
- [combat_formulas.md](./combat_formulas.md)
- [combat_data_tables.md](./combat_data_tables.md)
- [boss_pattern.md](./boss_pattern.md)
- [monster_ai.md](./monster_ai.md)
- [../common/skill.md](../common/skill.md)
- [../common/item.md](../common/item.md)
