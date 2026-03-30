# 전투 공식 초안

## 목적

전투 스탯 초안을 기반으로 우리 게임에서 사용할 전투 계산 구조를 정의한다.

## 핵심 규칙

- 이 문서는 PvE 기준 공식 초안이다.
- 실제 밸런스 수치는 후속 엑셀에서 조정하되, 계산 구조와 입력 스탯은 여기서 먼저 고정한다.
- 공식은 서버 권위 판정과 틱 단위 검증을 전제로 한다.
- 무기 기반 정체성, 패링 중심 전투, 무력화, 부위 파괴, 속성 상호작용, 위협도 운용을 모두 표현할 수 있어야 한다.

## 공통 표기

- `A`: 공격자 스탯
- `D`: 방어자 스탯
- `SkillCoeff`: 스킬 계수
- `Base`: 계산 중간값
- `Clamp(x, min, max)`: 최소/최대 범위 제한
- `Final`: 최종 결과값

## 기반 공식

### 1. 기본 공격력

```text
BaseAttack = A.AttackPower + A.WeaponPower
```

- 플레이어의 공격 성장은 `AttackPower`와 `WeaponPower`의 합을 기준으로 본다.
- 후속 단순화가 필요하면 둘 중 하나를 제거할 수 있지만, 현재는 캐릭터 성장과 무기 성장을 분리하기 위해 두 축을 유지한다.

### 2. 스킬 기본 피해

```text
SkillBaseDamage = BaseAttack * SkillCoeff + A.SkillPower
PositionMultiplier = 1 + A.PositionalBonus
CriticalMultiplier = CritSuccess ? (1 + BaseCritDamage + A.CriticalDamage) : 1
DamageBeforeDefense = SkillBaseDamage * PositionMultiplier * CriticalMultiplier
```

- 평타는 낮은 `SkillCoeff`, 핵심 스킬은 높은 `SkillCoeff`를 사용한다.
- `SkillPower`는 스킬 특화 성장이나 버프 효과를 실을 수 있는 보정 슬롯이다.

### 3. 방어 및 최종 피해

```text
DefenseMitigation = D.Defense / (D.Defense + DefenseK)
FinalDamage = DamageBeforeDefense * (1 - DefenseMitigation) * (1 - D.DamageReduction)
FinalDamage = max(1, FinalDamage)
```

- 방어는 감쇠형 공식을 사용해 값이 커질수록 효율 증가가 둔화되게 한다.
- `DamageReduction`은 버프, 실드, 패턴 특수 효과 같은 최종 단계 보정치에 사용한다.

### 4. 치명타

```text
CritRate = Clamp(BaseCritRate + A.CriticalChance, CritMin, CritMax)
CritSuccess = Roll01() < CritRate
```

- 치명타는 단순 확률 + 배율 구조로 유지한다.
- 현재 PvE 기준에서는 별도 치명 저항을 두지 않고 필요 시 보스 데이터에서만 예외 처리한다.

### 5. 속성 피해

```text
ElementDelta = A.ElementAttack_X - D.ElementResist_X
ElementMultiplier = 1 + Clamp(ElementDelta / ElementScale, ElementMin, ElementMax)
WeaknessMultiplier = IsWeakElement ? (1 + A.WeakPointPower) : 1
FinalElementDamage = FinalDamage * ElementMultiplier * WeaknessMultiplier
```

- `X`는 화염, 냉기, 번개, 독 중 하나다.
- 현재 보스 기획 기준 약점 속성은 추가 피해 배율, 저항 속성은 감소 배율로 처리한다.
- 완전 무효보다는 증감 배율 구조가 협동 액션 RPG 흐름과 잘 맞는다.

### 6. 무력화

```text
StaggerBase = SkillStaggerCoeff * (A.StaggerPower + BaseAttack * StaggerAttackRatio)
StaggerMitigation = D.StaggerResistance / (D.StaggerResistance + StaggerK)
StaggerApplied = StaggerBase * (1 - StaggerMitigation)
StaggerApplied = StaggerApplied * PatternBonus * CounterBonus * WeaknessMultiplier
D.StaggerGauge = max(0, D.StaggerGauge - StaggerApplied)
```

- 무력화는 데미지와 분리된 별도 게이지를 사용한다.
- 패링, 카운터, 약점 속성 적중은 무력화에 추가 보너스를 준다.
- 보스는 무력화 후 `StaggerRecovery` 또는 누적 저항 상승으로 연속 다운을 방지한다.

### 7. 부위 파괴

```text
PartBreakBase = SkillPartBreakCoeff * A.PartBreakPower
PartBreakMitigation = D.PartBreakResistance / (D.PartBreakResistance + PartBreakK)
PartBreakApplied = PartBreakBase * (1 - PartBreakMitigation)
TargetPartGauge = max(0, TargetPartGauge - PartBreakApplied)
```

- 부위 파괴는 전체 HP와 분리된 부위별 게이지를 사용한다.
- 성공 시 패턴 약화, 기술 봉쇄, 무력화 추가 기여 중 하나를 제공한다.

### 8. 상태 이상 축적

```text
StatusBuildBase = SkillStatusCoeff * (A.StatusBuildUp + A.ElementAttack_X)
StatusMitigation = D.StatusResistance / (D.StatusResistance + StatusK)
StatusApplied = StatusBuildBase * (1 - StatusMitigation)
TargetStatusGauge = TargetStatusGauge + StatusApplied

If TargetStatusGauge >= StatusThreshold:
    ApplyStatus()
    TargetStatusGauge = 0
    D.StatusResistance = D.StatusResistance + StatusResistStep
```

- 상태 이상은 확률 즉발형보다 누적 게이지형을 사용한다.
- 반복 발동 시 저항이 상승하도록 해 보스전에서 제어 루프를 완화한다.

### 9. 위협도

```text
ThreatFromDamage = FinalElementDamage * ThreatDamageRatio
ThreatFromSkill = SkillThreatBase
ThreatFromTaunt = TauntValue
ThreatGenerated = (ThreatFromDamage + ThreatFromSkill + ThreatFromTaunt) * (1 + A.ThreatGen)

TargetThreat[A] = TargetThreat[A] + ThreatGenerated
TargetThreat[A] = TargetThreat[A] * (1 - ThreatDecayPerTick)
```

- 탱커형 스킬은 별도 고정 위협값 또는 도발값을 가진다.
- 강제 타게팅 패턴은 위협도 계산보다 우선한다.

### 10. 아이덴티티 게이지

```text
IdentityGain = HitBaseGauge * SkillGaugeCoeff * (1 + A.IdentityGaugeGain)
OnHit:
    A.IdentityGauge = min(A.IdentityGaugeMax, A.IdentityGauge + IdentityGain)

OnIdentityUse:
    A.IdentityGauge = max(0, A.IdentityGauge - IdentityCost)
```

- 무기별 차이는 `HitBaseGauge`, `SkillGaugeCoeff`, `IdentityCost` 조합으로 표현한다.
- 평타, 핵심 스킬, 패링 성공에 서로 다른 게이지 보너스를 줄 수 있다.

### 11. 자원과 쿨다운

```text
FinalCooldown = BaseCooldown / (1 + A.CooldownReduction)
FinalResourceCost = BaseResourceCost * CostMultiplier
ResourceAfterUse = CurrentResource - FinalResourceCost
```

- 쿨다운 감소는 분모형 구조를 사용해 과도한 100% 근접을 막는다.
- 자원 소모는 기본적으로 선형으로 두고 특정 버프 상태에서만 예외 배율을 붙인다.

### 12. 공속, 시전 속도, 이동 속도

```text
FinalAttackInterval = BaseAttackInterval / (1 + A.AttackSpeed)
FinalCastTime = BaseCastTime / (1 + A.CastSpeed)
FinalMoveSpeed = BaseMoveSpeed * (1 + A.MoveSpeed)
```

- 패링 중심 전투에서는 이동 속도와 시전 속도의 체감 영향이 크므로 상한선을 둘 가능성이 높다.

## 판정 규칙

### 패링

```text
ParrySuccess = AttackContactTime within [ParryStart, ParryEnd + A.ParryWindowBonus]
If ParrySuccess:
    NegateOrReduceDamage()
    AddStaggerBonus()
    OpenCounterWindow()
```

- 패링은 단순 회피가 아니라 보상형 방어 판정이다.
- 성공 시 피해 무효 또는 큰 감소, 무력화 보너스, 카운터 창 생성 중 최소 2개를 제공한다.

### 카운터 및 캔슬

```text
CanCounter = IncomingPattern.CounterTag matches Skill.CounterTag
CanCancel = IncomingCast.CancelGrade <= Skill.CancelGrade
```

- `Heavy`, `Crush` 같은 캔슬 등급은 보스 문서 규칙을 따른다.
- 카운터 성공 시 추가 무력화 보너스를 적용한다.

## 후속 확정 항목

- `DefenseK`, `StaggerK`, `PartBreakK`, `StatusK`, `ElementScale` 같은 상수
- 치명타 최소/최대치와 공속/이속 상한선
- 상태 이상 종류별 `StatusThreshold`와 `StatusResistStep`
- 무기 타입별 `SkillCoeff`, `SkillStaggerCoeff`, `SkillPartBreakCoeff`, `SkillGaugeCoeff`

## 관련 문서

- [combat.md](./combat.md)
- [combat_stats.md](./combat_stats.md)
- [boss_pattern.md](./boss_pattern.md)
- [monster_ai.md](./monster_ai.md)
- [../common/skill.md](../common/skill.md)
- [../common/item.md](../common/item.md)
