# 전투 스탯 초안

## 목적

현재 기획서에 정의된 전투 루프, 스킬 구조, 장비 옵션, 보스 규칙을 기준으로 전투 스탯의 공통 목록을 정리한다.

## 핵심 규칙

- 이 문서는 수치 확정 문서가 아니라 스탯 목록과 역할 정의를 위한 초안이다.
- 스탯은 플레이어, 몬스터, 보스, 장비, 스킬이 공통으로 참조할 수 있는 전투 언어를 만드는 것이 목적이다.
- 상세 공식은 후속 밸런스 문서 또는 구현 문서에서 확정하되, 어떤 스탯이 필요한지는 여기서 먼저 고정한다.
- 무기별 정체성, 패링, 무력화, 부위 파괴, 속성 상호작용, 위협도 운용을 표현할 수 있어야 한다.

## 주요 스탯 목록

### 1. 생존 계열

- `MaxHP`: 체력 최대치. 플레이어, 몬스터, 보스의 기본 생존력을 정의한다.
- `HPRegen` 또는 `RecoverPower`: 초당 회복 또는 회복 효과 증폭. 현재 기획에서는 비중이 낮지만 소모품과 지원 스킬 확장을 고려해 확보한다.
- `Defense`: 일반 피해 완화용 기본 방어 계열 값. 정확한 공식은 미확정이다.
- `DamageReduction`: 스킬, 버프, 장비 옵션으로 얻는 최종 피해 감소 보정치.
- `GuardPower`: 검방 같은 방어형 무기의 가드 성능 보정치.
- `ParryWindowBonus`: 패링 가능 시간 확대 또는 난이도 완화 보정치.
- `StatusResistance`: 상태 이상 축적 저항 또는 지속 시간 감소에 쓰는 저항 계열의 공통 슬롯.

### 2. 공격 계열

- `AttackPower`: 기본 피해 계산의 중심이 되는 공격 계열 값.
- `WeaponPower`: 무기 기반 올인 빌드를 반영하기 위한 무기 성능 축. `AttackPower`와 분리할지 합칠지는 후속 수식에서 결정한다.
- `SkillPower`: 스킬 계수형 피해와 보조 효과의 기반 값.
- `CriticalChance`: 치명타 발생 확률. 현재 문서에 직접 언급되지는 않았지만 드롭 기반 성장과 장비 옵션 확장을 위해 후보군에 포함한다.
- `CriticalDamage`: 치명타 성공 시 추가 피해 배율.
- `WeakPointPower`: 약점 속성 또는 약점 상태 대상에게 주는 피해 보정치.
- `BackAttackBonus` 또는 `PositionalBonus`: 위치 선정이 중요한 전투 흐름을 반영하는 후방/측면 보정치 후보.

### 3. 자원 및 템포 계열

- `IdentityGaugeGain`: 공격 적중 시 아이덴티티 게이지 획득량.
- `IdentityGaugeMax`: 아이덴티티 발동에 필요한 최대 게이지.
- `ResourceMax`: 마나, 기력, 탄환 같은 무기별 자원의 최대치.
- `ResourceRegen`: 전투 중 자원 회복 속도.
- `CooldownReduction`: 스킬 재사용 대기시간 단축 계열.
- `CastSpeed`: 시전형 스킬의 준비 속도.
- `AttackSpeed`: 기본 공격과 일부 스킬의 실행 속도.
- `MoveSpeed`: 회피와 포지셔닝 중심 전투를 위한 이동 속도.
- `DodgeCharge` 또는 `DodgeCount`: 회피기 사용 가능 횟수 또는 충전 수.

### 4. 전투 기믹 계열

- `StaggerPower`: 무력화 게이지 감소량에 기여하는 공격 성능.
- `StaggerResistance`: 무력화에 대한 저항. 주로 보스나 정예 몬스터가 사용한다.
- `StaggerRecovery`: 무력화 저항 회복 또는 누적 저항 관리에 쓰는 값.
- `PartBreakPower`: 부위 파괴 게이지 감소량에 기여하는 성능.
- `PartBreakResistance`: 부위 파괴 저항치.
- `CounterPower`: 카운터 성공 시 추가 기여도나 경직 유발 성능.
- `ThreatGen`: 탱커형 스킬과 장비가 생성하는 위협도 보정치.
- `ThreatDecay`: 시간 경과 또는 상태 전환에 따른 위협도 감소율.
- `ControlPower`: 넉백, 경직, 띄우기 같은 제어 성능의 강도.
- `ControlResistance`: 반복 제어 저항과 보스 면역 규칙을 위한 값.

### 5. 속성 및 상태 이상 계열

- `ElementAttack_Fire`
- `ElementAttack_Ice`
- `ElementAttack_Lightning`
- `ElementAttack_Poison`
- `ElementResist_Fire`
- `ElementResist_Ice`
- `ElementResist_Lightning`
- `ElementResist_Poison`
- `StatusBuildUp`: 상태 이상 축적량 증가 보정.
- `StatusDuration`: 부여한 상태 이상의 지속 시간 보정.
- `StatusAmplify`: 이미 걸린 상태 이상 대상에게 추가 효과를 주는 보정치 후보.

### 6. 보스 및 몬스터 전용 계열

- `MaxHPPhaseThreshold`: 체력 구간 기반 페이즈 전환 기준값.
- `EnrageTime`: 광폭화 발동 시간.
- `DetectionRange`: 일반 몬스터의 탐지 범위.
- `AggroPriority`: 위협도 테이블 외 우선 타게팅 보정치.
- `PatternWeight`: AI가 특정 패턴을 선택할 확률 가중치.
- `StatusImmunityFlag`: 특정 상태 이상 면역 여부.
- `ForcedTargetingFlag`: 기믹 구간 강제 타게팅 사용 여부.

### 7. 장비/드롭 연동 계열

- `Durability`: 사망 패널티와 수리 비용의 기준이 되는 내구도.
- `DurabilityLossOnDeath`: 사망 시 내구도 감소량.
- `RepairCostRate`: 수리 비용 계산 계수.
- `OptionSlotCount`: 아이템 등급별 랜덤 옵션 수.
- `SmartDropWeight`: 장착 무기 기준 스마트 드롭 보정치.
- `TradeLimitCount`: 거래 가능 횟수.

## 스탯 묶음 초안

### 플레이어 기본 스탯

- `MaxHP`
- `AttackPower`
- `Defense`
- `MoveSpeed`
- `IdentityGaugeMax`
- `IdentityGaugeGain`
- `ResourceMax`
- `ResourceRegen`

### 전투 보조 스탯

- `CooldownReduction`
- `AttackSpeed`
- `CastSpeed`
- `CriticalChance`
- `CriticalDamage`
- `ThreatGen`
- `ParryWindowBonus`
- `StatusResistance`

### 특수 전투 스탯

- `StaggerPower`
- `PartBreakPower`
- `CounterPower`
- `ControlPower`
- `WeakPointPower`
- 속성 공격/저항

### 장비 옵션 후보 스탯

- `AttackPower`
- `Defense`
- `MaxHP`
- `StaggerPower`
- `PartBreakPower`
- `ParryWindowBonus`
- `ThreatGen`
- 속성 공격/저항
- `CooldownReduction`
- `CriticalChance`
- `CriticalDamage`

## 제약과 예외

- `CriticalChance`, `CriticalDamage`, `BackAttackBonus` 같은 스탯은 현재 문서에서 직접 요구되지는 않지만 장비 옵션과 성장 다양성 확보를 위한 후보군으로 둔다.
- `AttackPower`와 `WeaponPower`를 분리할지, `Defense`와 `DamageReduction`을 별도 관리할지는 후속 공식 문서에서 확정한다.
- 상태 이상 종류 자체는 아직 별도 문서가 없으므로 현재는 공통 슬롯만 정의한다.
- 수식, 계수, 상한선, 최소값, 내림/반올림 규칙은 구현 또는 밸런스 문서에서 확정한다.

## 관련 문서

- [combat_formulas.md](./combat_formulas.md)
- [combat.md](./combat.md)
- [boss_pattern.md](./boss_pattern.md)
- [monster_ai.md](./monster_ai.md)
- [../common/skill.md](../common/skill.md)
- [../common/item.md](../common/item.md)
