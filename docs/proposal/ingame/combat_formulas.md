# 전투 공식 초안

## 목적

전투 스탯 초안을 기반으로 데미지, 무력화, 속성, 상태 이상, 위협도, 자원 운영의 기본 계산 구조를 정의한다.

## 핵심 규칙

- 이 문서는 기획 단계용 공식 초안이며, 실제 계수 값과 상한선은 후속 밸런스 조정 대상이다.
- 공식은 서버 권위 판정과 틱 단위 검증을 전제로 한다.
- 계산식은 무기 기반 정체성, 패링 중심 전투, 무력화/부위 파괴, 속성 상호작용을 표현할 수 있어야 한다.
- 지나치게 폭증하는 성장 곡선을 피하기 위해 퍼센트형 방어와 저항은 감쇠 구조를 우선 사용한다.

## 공통 표기

- `A`: 공격자 스탯
- `D`: 방어자 스탯
- `SkillCoeff`: 스킬 계수
- `Base`: 계산 중간값
- `Clamp(x, min, max)`: 최소/최대 범위 제한
- `Final`: 최종 결과값

## 주요 공식

### 1. 기본 피해

```text
BaseAttack = A.AttackPower + A.WeaponPower
SkillBaseDamage = BaseAttack * SkillCoeff + A.SkillPower
PositionBonus = 1 + A.PositionalBonus
CriticalBonus = CritSuccess ? A.CriticalDamage : 0
DamageBeforeDefense = SkillBaseDamage * PositionBonus * (1 + CriticalBonus)
DefenseMitigation = D.Defense / (D.Defense + DefenseK)
FinalDamage = DamageBeforeDefense * (1 - DefenseMitigation) * (1 - D.DamageReduction)
FinalDamage = max(1, FinalDamage)
```

- `DefenseK`는 방어 효율 곡선을 정하는 상수다.
- `AttackPower`와 `WeaponPower`를 합산할지, 무기 계수를 따로 둘지는 밸런스 단계에서 재검토한다.
- 평타와 스킬 모두 같은 골격을 쓰되, 평타는 더 낮은 `SkillCoeff`를 사용한다.

### 2. 치명타 판정

```text
CritRate = Clamp(BaseCritRate + A.CriticalChance - D.CritResist, CritMin, CritMax)
CritSuccess = Roll01() < CritRate
CriticalDamageMultiplier = 1 + BaseCritDamage + A.CriticalDamage
```

- 현재 문서에는 `CritResist`가 없지만, PvE 보스 튜닝 여지를 위해 내부 슬롯으로 유지할 수 있다.
- 치명타를 단순 배율형으로 두면 엑셀과 구현 양쪽에서 검증이 쉽다.

### 3. 속성 피해

```text
ElementDelta = A.ElementAttack_X - D.ElementResist_X
ElementMultiplier = 1 + Clamp(ElementDelta / ElementScale, ElementMin, ElementMax)
WeaknessBonus = IsWeakElement ? 1 + A.WeakPointPower : 1
FinalElementDamage = FinalDamage * ElementMultiplier * WeaknessBonus
```

- `X`는 화염, 냉기, 번개, 독 중 하나다.
- 보스 약점 속성은 [boss_pattern.md](./boss_pattern.md) 기준으로 별도 설정한다.
- 속성 상성은 "완전 무효"보다 증감 배율 구조가 현재 기획 톤에 더 잘 맞는다.

### 4. 무력화

```text
StaggerBase = SkillStaggerCoeff * (A.StaggerPower + BaseAttack * StaggerAttackRatio)
StaggerMitigation = D.StaggerResistance / (D.StaggerResistance + StaggerK)
StaggerApplied = StaggerBase * (1 - StaggerMitigation)
StaggerApplied = StaggerApplied * PatternBonus * CounterBonus * WeaknessBonus
D.StaggerGauge = max(0, D.StaggerGauge - StaggerApplied)
```

- 패링 성공, 카운터 성공, 약점 속성 적중은 `PatternBonus` 또는 `CounterBonus`로 추가 보정한다.
- 보스는 무력화 이후 일정 시간 `StaggerRecovery` 또는 누적 저항 증가를 적용해 연속 다운을 완화한다.

### 5. 부위 파괴

```text
PartBreakBase = SkillPartBreakCoeff * A.PartBreakPower
PartBreakMitigation = D.PartBreakResistance / (D.PartBreakResistance + PartBreakK)
PartBreakApplied = PartBreakBase * (1 - PartBreakMitigation)
TargetPartGauge = max(0, TargetPartGauge - PartBreakApplied)
```

- 부위 파괴는 전체 HP와 분리된 별도 게이지로 관리한다.
- 파괴 성공 시 패턴 약화, 특정 기술 봉쇄, 추가 무력화 기여 중 하나를 부여한다.

### 6. 상태 이상 축적

```text
StatusBuildBase = SkillStatusCoeff * (A.StatusBuildUp + A.ElementAttack_X)
StatusResistanceMitigation = D.StatusResistance / (D.StatusResistance + StatusK)
StatusApplied = StatusBuildBase * (1 - StatusResistanceMitigation)
TargetStatusGauge = TargetStatusGauge + StatusApplied

If TargetStatusGauge >= StatusThreshold:
    ApplyStatus()
    TargetStatusGauge = 0
    D.StatusResistanceStack += ResistanceStep
```

- 상태 이상은 즉시 확률 발동보다 누적 게이지 방식이 서버 검증과 보스전 설계에 유리하다.
- 보스는 발동할수록 `StatusResistanceStack`이 올라 반복 제어와 반복 상태 이상을 완화한다.
- 실제 상태 이상 종류는 후속 문서에서 확정하되, 구조는 공통으로 유지한다.

### 7. 위협도

```text
ThreatFromDamage = FinalElementDamage * ThreatDamageRatio
ThreatFromSkill = SkillThreatBase
ThreatFromTaunt = TauntValue
ThreatGenerated = (ThreatFromDamage + ThreatFromSkill + ThreatFromTaunt) * (1 + A.ThreatGen)

TargetThreat[A] = TargetThreat[A] + ThreatGenerated
TargetThreat[A] = TargetThreat[A] * (1 - ThreatDecayPerTick)
```

- 탱커형 무기와 스킬은 `SkillThreatBase` 또는 `TauntValue`로 차별화한다.
- 강제 타게팅 패턴은 위협도 계산보다 우선한다.

### 8. 아이덴티티 게이지

```text
IdentityGain = HitBaseGauge * SkillGaugeCoeff * (1 + A.IdentityGaugeGain)
OnHit:
    A.IdentityGauge = min(A.IdentityGaugeMax, A.IdentityGauge + IdentityGain)

OnIdentityUse:
    A.IdentityGauge = max(0, A.IdentityGauge - IdentityCost)
```

- 무기별 차이는 `HitBaseGauge`, `SkillGaugeCoeff`, `IdentityCost` 조합으로 표현한다.
- 강한 순간 성능을 강조하려면 평타보다 특정 스킬의 게이지 획득 효율을 높일 수 있다.

### 9. 자원과 쿨다운

```text
FinalCooldown = BaseCooldown / (1 + A.CooldownReduction)
FinalResourceCost = BaseResourceCost * CostMultiplier
ResourceAfterUse = CurrentResource - FinalResourceCost
```

- 쿨다운 감소는 뺄셈형보다 분모형이 과도한 100% 근접을 막기 쉽다.
- 자원 소모는 기본적으로 선형으로 두고, 특수 상태에서만 예외 배율을 붙인다.

### 10. 공속과 시전 속도

```text
FinalAttackInterval = BaseAttackInterval / (1 + A.AttackSpeed)
FinalCastTime = BaseCastTime / (1 + A.CastSpeed)
FinalMoveSpeed = BaseMoveSpeed * (1 + A.MoveSpeed)
```

- 패링 중심 전투에서는 공속보다 시전 속도와 이동 속도의 체감 영향이 더 크므로 상한선 관리가 필요하다.

## 판정 규칙 초안

### 패링

```text
ParrySuccess = AttackContactTime within [ParryStart, ParryEnd + A.ParryWindowBonus]
If ParrySuccess:
    NegateOrReduceDamage()
    ApplyCounterWindow()
    AddStaggerBonus()
```

- 패링은 단순 회피가 아니라 보상형 방어 판정이다.
- 성공 시 피해 무효 또는 큰 감소, 카운터 가능 창, 무력화 추가 기여 중 최소 2개를 제공한다.

### 카운터/캔슬

```text
CanCounter = IncomingPattern.CounterTag matches Skill.CounterTag
CanCancel = IncomingCast.CancelGrade <= Skill.CancelGrade
```

- `Heavy`, `Crush` 같은 캔슬 등급은 보스 문서 규칙을 따른다.
- 카운터 성공 시 무력화와 위협도에 추가 보정을 줄 수 있다.

## 제약과 예외

- `DefenseK`, `StaggerK`, `StatusK`, `ElementScale` 같은 상수값은 현재 비워 두고 엑셀 단계에서 채운다.
- `AttackPower`와 `WeaponPower`를 둘 다 유지할지 여부는 실제 아이템/성장 설계가 더 구체화되면 단순화할 수 있다.
- 치명타, 후방 공격, 회복력 같은 일부 스탯은 현재 기획 문서의 직접 요구보다 미래 확장성을 고려한 포함이다.
- PvP는 현재 범위 밖이므로 공식은 전부 PvE 기준이다.

## 관련 문서

- [combat.md](./combat.md)
- [combat_stats.md](./combat_stats.md)
- [boss_pattern.md](./boss_pattern.md)
- [monster_ai.md](./monster_ai.md)
- [../common/skill.md](../common/skill.md)
- [../common/item.md](../common/item.md)
