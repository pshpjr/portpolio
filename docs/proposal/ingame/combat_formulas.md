# 전투 공식 초안

## 목적

전투 스탯 초안을 기반으로 포트폴리오 1차 구현에서 실제로 재현 가능한 전투 계산 구조를 정의한다.

## 핵심 규칙

- 이 문서는 PvE 기준 구현용 `v1 코어 공식`만 우선 고정한다.
- 서버는 30fps 틱 루프 기준으로 전투 검증을 수행한다.
- 계산 순서와 입력 필드는 이 문서를 기준으로 고정하고, 복잡한 파생 규칙은 후속 확장으로 넘긴다.
- 1차 전투 코어는 `피해`, `치명타`, `무력화`, `아이덴티티`, `패링/카운터`다.

## 공통 표기

- `A`: 공격자 스탯
- `D`: 방어자 스탯
- `SkillCoeff`: 스킬 계수
- `Base`: 계산 중간값
- `Clamp(x, min, max)`: 최소/최대 범위 제한
- `Final`: 최종 결과값

## 최종 전투 스탯 조립

### 조립 식

```text
BaseFromLevel = PlayerLevelStats
BaseFromWeapon = WeaponBaseStats
BaseFromEquipment = ArmorFixedStats + AccessoryFixedStats
BaseFromOptions = EquipmentOptionStats
BaseFromTemporary = PassiveStats + BuffStats - DebuffStats

FinalCombatStat =
    Clamp(
        BaseFromLevel
        + BaseFromWeapon
        + BaseFromEquipment
        + BaseFromOptions
        + BaseFromTemporary,
        StatMin,
        StatCap
    )
```

- 레벨은 캐릭터 공통 성장축이다.
- 무기는 공격 리듬과 자원 구조를 정한다.
- 방어구/장신구는 최종값을 보완한다.
- 임시 효과는 항상 가장 마지막에 합산/감산한다.
- 실제 상한은 [combat_stats.md](./combat_stats.md)의 `v1 상한선`을 따른다.

### 전투 시작 전 스냅샷 원칙

- 입장 시점 또는 장비 변경 확정 시 서버가 최종 전투 스탯 스냅샷을 다시 계산한다.
- 던전 중 장비 교체는 허용하지 않으므로, 전투 중 갱신 대상은 버프/디버프와 자원 관련 값이 중심이다.
- 수리 필요 여부, 귀속 상태, 옵션 재련 결과는 전투 공식이 아니라 아이템 상태 검증에서 먼저 확정한다.

## v1 기본 상수

| 상수 | 값 | 설명 |
|---|---:|---|
| `DefenseK` | 300 | 방어 감쇠 곡선 기준값 |
| `StaggerK` | 180 | 무력화 저항 감쇠 기준값 |
| `BaseCritRate` | 0.05 | 기본 치명타 확률 |
| `BaseCritDamage` | 0.50 | 기본 치명타 추가 피해 |
| `CritMin` | 0.00 | 치명타 하한 |
| `CritMax` | 0.40 | 치명타 상한 |
| `DefaultParryWindow` | 0.18 | 기본 패링 판정 시간(초) |
| `CounterWindow` | 1.20 | 패링/카운터 성공 후 카운터 가능 시간(초) |
| `BossDownTime` | 6.00 | 보스 무력화 성공 시 기본 다운 시간(초) |
| `ThreatDamageRatio` | 0.25 | 피해 1당 발생 위협도 비율 |

## 서버 처리 순서

1. 입력 커맨드와 캐릭터 상태를 검증한다.
2. 사거리, 각도, 시전 가능 상태, 자원 보유량, 쿨다운을 확인한다.
3. 타격 대상과 충돌 시점을 계산한다.
4. 패링, 카운터, 가드 같은 특수 판정을 먼저 처리한다.
5. 피해, 치명타, 무력화, 아이덴티티 게이지 변경을 순서대로 적용한다.
6. HP/게이지/위협도 변경 이벤트를 확정하고 브로드캐스트한다.

## v1 기반 공식

### 1. 기본 공격력

```text
BaseAttack = A.AttackPower + A.WeaponPower
```

- `AttackPower`는 유저 레벨 성장 축, `WeaponPower`는 장착 무기 축으로 유지한다.
- 기본 공격 한 번당 기준 피해 목표는 같은 레벨 일반 몬스터 HP의 `6% ~ 9%`다.

### 2. 스킬 기본 피해

```text
SkillBaseDamage = BaseAttack * SkillCoeff
CriticalMultiplier = CritSuccess ? (1 + BaseCritDamage + A.CriticalDamage) : 1
DamageBeforeDefense = SkillBaseDamage * CriticalMultiplier
```

- `SkillCoeff`는 스킬 데이터에서 관리한다.
- `SkillPower`, `PositionalBonus` 같은 추가 배율 계열은 후속 확장으로 넘긴다.

### 스킬 계수 기준

| 스킬 등급 | `SkillCoeff` | 설명 |
|---|---:|---|
| 기본 공격 1타 | 0.70 | 필러 공격 |
| 기본 공격 피니시 | 1.10 | 평타 마무리 |
| 일반 액티브 | 1.40 | 주력 스킬 |
| 강한 액티브 | 1.90 | 긴 쿨다운 고위력 |
| 아이덴티티 스킬 | 2.40 | 무기 정체성 핵심기 |

### 무기별 시작 계수 가이드

| 스킬 | 검방 | 대검 | 지팡이 |
|---|---:|---:|---:|
| 기본 공격 | 0.72 | 0.68 | 0.64 |
| 일반 액티브 | 1.28 | 1.40 | 1.34 |
| 강한 액티브 | 1.70 | 1.98 | 1.82 |
| 아이덴티티 | 2.02 | 2.42 | 2.26 |

- 검방은 안정성과 패링 보상을 우선하고, 대검은 단일 강타 보상을 극대화한다.
- 지팡이는 강한 한 방보다 지속 회전과 리소스 운용을 더 잘 살리도록 잡는다.

### 3. 방어 및 최종 피해

```text
DefenseMitigation = D.Defense / (D.Defense + DefenseK)
FinalDamage = DamageBeforeDefense * (1 - DefenseMitigation) * (1 - D.DamageReduction)
FinalDamage = max(1, FinalDamage)
```

- 방어는 감쇠형 공식을 사용해 값이 커질수록 효율 증가가 둔화되게 한다.
- `DamageReduction`은 버프, 실드, 패턴 특수 효과 같은 최종 단계 보정치에 사용한다.
- 같은 레벨 기준 일반 몬스터 `Defense`는 `80`, 정예 `140`, 보스 `220`를 기본값으로 둔다.

### 4. 치명타

```text
CritRate = Clamp(BaseCritRate + A.CriticalChance, CritMin, CritMax)
CritSuccess = Roll01() < CritRate
```

- 치명타는 단순 확률 + 배율 구조로 유지한다.
- 현재 PvE 기준에서는 별도 치명 저항을 두지 않고 필요 시 보스 데이터에서만 예외 처리한다.

### 5. 무력화

```text
StaggerBase = SkillStaggerCoeff * A.StaggerPower
StaggerMitigation = D.StaggerResistance / (D.StaggerResistance + StaggerK)
StaggerApplied = StaggerBase * (1 - StaggerMitigation) * CounterBonus
D.StaggerGauge = max(0, D.StaggerGauge - StaggerApplied)
```

- 무력화는 데미지와 분리된 별도 게이지를 사용한다.
- `SkillStaggerCoeff`는 스킬 데이터에서 관리한다.
- `CounterBonus`는 일반 적중 `1.0`, 카운터 성공 `1.5`, 패링 직후 반격 `1.35`를 사용한다.

### 무력화 기준값

| 대상 | `StaggerGauge` | `StaggerResistance` |
|---|---:|---:|
| 일반 몬스터 | 120 | 40 |
| 정예 몬스터 | 350 | 90 |
| 보스 | 1000 | 180 |

| 스킬 분류 | `SkillStaggerCoeff` |
|---|---:|
| 낮음 | 0.7 |
| 보통 | 1.0 |
| 높음 | 1.4 |
| 매우 높음 | 1.8 |

### 무기별 무력화 계수 가이드

| 스킬 | 검방 | 대검 | 지팡이 |
|---|---:|---:|---:|
| 기본 공격 | 0.90 | 0.80 | 0.65 |
| 일반 액티브 | 1.05 | 1.00 | 0.85 |
| 강한 액티브 | 1.25 | 1.45 | 1.10 |
| 아이덴티티 | 1.50 | 1.80 | 1.30 |

- 검방은 자주 누적시키는 무력화, 대검은 한 번에 크게 깎는 무력화, 지팡이는 보조형 무력화로 구분한다.

### 무력화 상태 전이 규칙

| 단계 | 조건 | 효과 |
|---|---|---|
| `NORMAL` | 기본 상태 | 일반 패턴 수행 |
| `STAGGER_BREAK` | `StaggerGauge <= 0` | 즉시 현재 행동 중단 |
| `DOWNED` | `STAGGER_BREAK` 직후 | `BossDownTime = 6초`, 받는 피해 `+20%` |
| `RECOVERY` | 다운 종료 직후 | `StaggerGauge`를 최대치의 60%로 복구, 재무력화 불가 |
| `NORMAL` | `RECOVERY` 종료 | 일반 상태 복귀 |

- 1차 구현에서는 다운 시간과 회복 규칙만 공통값으로 두고, 연속 무력화 누적 저항은 후속 확장으로 넘긴다.

### 6. 위협도

```text
ThreatFromDamage = FinalDamage * ThreatDamageRatio
ThreatFromTaunt = TauntValue
ThreatGenerated = (ThreatFromDamage + ThreatFromTaunt) * (1 + A.ThreatGen)
TargetThreat[A] = TargetThreat[A] + ThreatGenerated
```

- 1차 구현에서는 `피해 기반 위협도 + 고정 도발값`만 사용한다.
- 힐, 보호막, 자연 감쇠, 최근 피격 보너스, 강제 타게팅 복귀 규칙은 후속 확장으로 넘긴다.

### 위협도 기준

| 행동 | 생성 위협도 |
|---|---:|
| 최종 피해 100 | 25 |
| 일반 도발 스킬 | 250 고정 |
| 강한 도발 스킬 | 400 고정 |

### 7. 아이덴티티 게이지

```text
IdentityGain = HitBaseGauge * SkillGaugeCoeff * (1 + A.IdentityGaugeGain)
OnHit:
    A.IdentityGauge = min(A.IdentityGaugeMax, A.IdentityGauge + IdentityGain)

OnIdentityUse:
    A.IdentityGauge = max(0, A.IdentityGauge - IdentityCost)
```

- 무기별 차이는 `HitBaseGauge`, `SkillGaugeCoeff`, `IdentityCost` 조합으로 표현한다.
- 위 값들은 스킬 데이터에서 관리한다.

### 아이덴티티 게이지 기준

| 행동 | `HitBaseGauge` |
|---|---:|
| 평타 1회 적중 | 35 |
| 일반 액티브 적중 | 70 |
| 강한 액티브 적중 | 110 |
| 패링 성공 | 150 |
| 카운터 성공 | 180 |

| 스킬 분류 | `SkillGaugeCoeff` |
|---|---:|
| 평타 | 1.0 |
| 일반 액티브 | 1.0 |
| 강한 액티브 | 1.1 |
| 광역 다단히트 | 0.65 |

### 무기별 게이지 계수 가이드

| 항목 | 검방 | 대검 | 지팡이 |
|---|---:|---:|---:|
| 기본 공격 | 1.05 | 0.95 | 1.00 |
| 일반 액티브 | 1.00 | 1.00 | 1.05 |
| 강한 액티브 | 1.00 | 1.05 | 1.10 |
| 다단히트 | 0.65 | 0.60 | 0.70 |
| 패링 | 1.30 | 1.10 | 1.00 |
| 카운터 | 1.45 | 1.25 | 1.10 |

| 무기 | `IdentityCost` |
|---|---:|
| 검방 | 900 |
| 대검 | 1000 |
| 지팡이 | 950 |

- 검방은 패링 성공 보상으로 게이지 순환을 자주 만들고, 대검은 표준 회전 위에 큰 보상기를 얹는다.
- 지팡이는 다단히트가 많더라도 과도한 충전이 나오지 않게 다단히트 계수를 더 낮게 둔다.

### 8. 자원과 쿨다운

```text
FinalCooldown = BaseCooldown / (1 + A.CooldownReduction)
FinalResourceCost = BaseResourceCost
ResourceAfterUse = CurrentResource - FinalResourceCost
```

- 쿨다운 감소는 분모형 구조를 사용해 과도한 100% 근접을 막는다.
- 자원 소모는 기본적으로 선형으로 두고, v1에서는 별도 코스트 배율 시스템을 두지 않는다.

### 자원/쿨다운 기준

| 스킬 분류 | 기본 쿨다운 | 기본 자원 소모 |
|---|---:|---:|
| 일반 액티브 | 8초 | 20 |
| 강한 액티브 | 16초 | 35 |
| 이동기 | 12초 | 15 |
| 아이덴티티 | 24초 | 0 |

## TTK 및 적 수치 기준선

### 설계 원칙

- 일반 몬스터는 `1 ~ 2사이클` 안에 정리돼 전투 템포를 끊지 않게 한다.
- 정예 몬스터는 `2 ~ 4사이클`을 요구해 패링, 무력화, 자원 운용이 한 번 이상 의미 있게 들어가게 한다.
- 보스는 `7 ~ 10사이클` 이상을 요구해 패턴 학습과 다운 타이밍 집중이 전투의 핵심이 되게 한다.
- 여기서 `1사이클`은 `평타 3 ~ 5타 + 일반 액티브 1회`를 기준으로 본다.

### 레벨 구간별 TTK 목표

| 레벨 | 일반 몬스터 | 정예 | 보스 |
|---|---|---|---|
| 1 | 검방 `4 ~ 6초`, 대검 `3 ~ 5초`, 지팡이 `3 ~ 5초` | 검방 `14 ~ 18초`, 대검 `12 ~ 16초`, 지팡이 `13 ~ 17초` | 검방 `90 ~ 120초`, 대검 `75 ~ 100초`, 지팡이 `85 ~ 110초` |
| 20 | 검방 `4 ~ 5초`, 대검 `3 ~ 4초`, 지팡이 `3 ~ 4초` | 검방 `15 ~ 20초`, 대검 `11 ~ 15초`, 지팡이 `12 ~ 16초` | 검방 `95 ~ 125초`, 대검 `80 ~ 105초`, 지팡이 `88 ~ 115초` |
| 40 | 검방 `3 ~ 5초`, 대검 `3 ~ 4초`, 지팡이 `3 ~ 4초` | 검방 `16 ~ 22초`, 대검 `12 ~ 16초`, 지팡이 `13 ~ 18초` | 검방 `100 ~ 130초`, 대검 `85 ~ 110초`, 지팡이 `90 ~ 120초` |

### 적 타입별 시작 수치 가이드

| 레벨 | 일반 몬스터 | 정예 | 보스 |
|---|---|---|---|
| 1 | `HP 500 ~ 650`, `Defense 80`, `StaggerGauge 120` | `HP 1400 ~ 1800`, `Defense 140`, `StaggerGauge 350` | `HP 9000 ~ 12000`, `Defense 220`, `StaggerGauge 1000` |
| 20 | `HP 950 ~ 1250`, `Defense 90`, `StaggerGauge 140` | `HP 2800 ~ 3600`, `Defense 160`, `StaggerGauge 420` | `HP 13000 ~ 16000`, `Defense 235`, `StaggerGauge 1100` |
| 40 | `HP 1600 ~ 2200`, `Defense 110`, `StaggerGauge 160` | `HP 4500 ~ 6000`, `Defense 190`, `StaggerGauge 500` | `HP 19000 ~ 25000`, `Defense 250`, `StaggerGauge 1200` |

- 이 표는 `1인 플레이 기준 v1 초기값`이다.
- `AttackPower + WeaponPower` 성장폭과 HP 성장폭을 비슷한 비율로 유지해 레벨이 올라가도 TTK가 크게 흔들리지 않게 한다.
- 파티 플레이에서는 기믹 수행과 다운 딜 집중 때문에 체감 TTK가 더 짧아질 수 있다.

### 9. 공속, 시전 속도, 이동 속도

```text
FinalAttackInterval = BaseAttackInterval / (1 + A.AttackSpeed)
FinalCastTime = BaseCastTime / (1 + A.CastSpeed)
FinalMoveSpeed = A.BaseMoveSpeed * (1 + A.MoveSpeedBonus)
```

- `BaseMoveSpeed`와 `MoveSpeedBonus`는 분리 유지한다.
- `AttackSpeed`, `CastSpeed`, `MoveSpeedBonus`는 각각 최종값이 기본값 대비 `+40%`, `+50%`, `+25%`를 넘지 못한다.

## v1 판정 규칙

### 패링

```text
ParrySuccess = AttackContactTime within [ParryStart, ParryEnd + A.ParryWindowBonus]
If ParrySuccess:
    NegateDamageForThisHitOnly()
    OpenCounterWindow()
```

- 패링은 단순 회피가 아니라 보상형 방어 판정이다.
- 1차 구현에서는 패링 성공한 해당 공격 1건의 피해만 `100%` 무효화한다.
- 패링 성공 시 `1.20초` 동안 카운터 가능 창을 연다.

### 패링 세부 규칙

- 기본 패링 판정 시간은 `0.18초`다.
- 검방은 `+0.04초`, 대검은 `+0.00초`, 지팡이는 `-0.02초`를 기본 직업 보정으로 가진다.
- 1차 구현에서는 단일 히트 기준 패링만 보장하고, 다단히트/동시 공격/투사체 우선순위 세부 규칙은 후속 확장으로 넘긴다.

### 카운터 및 캔슬

```text
CanCounter = IncomingPattern.CounterTag matches Skill.CounterTag
CanCancel = IncomingCast.CancelGrade <= Skill.CancelGrade
```

- 카운터 성공 시 추가 무력화 보너스를 적용한다.
- 캔슬 등급과 태그 체계는 보스 문서와 함께 맞춘다.

## 확장으로 넘기는 항목

- `SkillPower`, `PositionalBonus`, `WeakPointPower` 기반 추가 배율
- 속성 약점/저항의 세부 상성 규칙
- 상태 이상 축적, 상태 저항 누적, 상태별 지속시간 보정
- 위협도 자연 감쇠, 힐/보호막 위협, 최근 피격 보너스
- 다중 히트 패링 잠금, `Parryable`/`Unparryable` 태그 우선순위
- 연속 무력화 누적 저항

## 구현용 예시

### 대검 액티브가 보스를 타격한 경우

```text
가정:
- A.AttackPower = 95
- A.WeaponPower = 85
- SkillCoeff = 1.9
- A.CriticalChance = 0.08
- A.CriticalDamage = 0.60
- D.Defense = 220
- D.DamageReduction = 0.00

계산:
BaseAttack = 180
SkillBaseDamage = 180 * 1.9 = 342
DefenseMitigation = 220 / (220 + 300) = 0.423
방어 적용 후 피해 = 342 * (1 - 0.423) = 197.3
최종 확정 피해 = 197
```

- 이 예시는 같은 레벨 보스 상대로 강한 액티브 1회의 기준 피해량을 보여준다.
- 치명타가 발생하면 여기서 추가 배율을 곱한다.

## 후속 확정 항목

- 무기 타입별 `SkillCoeff`, `SkillStaggerCoeff`, `SkillGaugeCoeff`의 실제 스킬별 배분
- 각 던전 티어별 일반/정예/보스 HP 목표치
- 스킬별 자원 소모량 개별 오버라이드 값
- 확장 단계 상태 이상/속성 시스템 범위

## 관련 문서

- [combat.md](./combat.md)
- [combat_stats.md](./combat_stats.md)
- [boss_pattern.md](./boss_pattern.md)
- [monster_ai.md](./monster_ai.md)
- [../common/skill.md](../common/skill.md)
- [../common/item.md](../common/item.md)
