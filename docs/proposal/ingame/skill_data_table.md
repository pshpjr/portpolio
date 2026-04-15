# 스킬 데이터 테이블 구조

## 목적

스킬 정의 데이터의 테이블 구조와 책임 범위를 정의한다. 실제 행 데이터는 별도 JSON에서 관리한다.

## 핵심 규칙

- 스킬 **정의**(공식·판정)는 `SkillTable`이 담당한다.
- 무기 타입의 **기본 지급 스킬 세트**는 `WeaponDefaultSkillTable`이 담당한다.
- 캐릭터가 **해금한 스킬 목록**은 `CharacterSkillUnlockTable`이 담당한다.
- 캐릭터가 **현재 장착한 슬롯 배정**은 `CharacterSkillPresetTable`이 담당한다.
- 레벨별 성장 보정은 `SkillLevelTable`이 담당한다.
- 스킬 변형(트라이포드) 옵션은 `SkillTripoTable`이 담당한다.

## 테이블 관계 다이어그램

```
WeaponType
  → WeaponDefaultSkillTable.WeaponType  (무기 타입별 초기 지급 스킬 4종)
    → SkillTable.SkillId                (각 스킬의 판정·계수 정의)

Character.CharacterId + WeaponType
  → CharacterSkillUnlockTable           (해금된 스킬 목록)
    → SkillTable.SkillId

Character.CharacterId + WeaponType + SlotKey
  → CharacterSkillPresetTable           (슬롯별 장착 스킬)
    → SkillTable.SkillId

SkillTable.SkillId
  → SkillLevelTable.SkillId             (레벨별 계수 보정)
  → SkillTripoTable.SkillId             (트라이포드 변형)
```

## 테이블 구조

### `SkillTable` — 스킬 기본 정의

> SkillTable은 스킬 한 종의 판정 형태·계수·쿨다운·자원 소모를 저장하며, 서버가 스킬 발동 시 단일 조회로 전투 계산에 필요한 모든 수치를 가져올 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `SkillId` | INT | PK | |
| `SkillCode` | VARCHAR | DATA | 내부 코드명 (예: `SWORD_SHIELD_Q`) |
| `SkillName` | VARCHAR | DATA | 표시 이름 |
| `WeaponType` | ENUM | DATA | `SWORD_SHIELD`, `GREAT_SWORD`, `STAFF` |
| `SlotCategory` | ENUM | DATA | `BASIC_ATTACK`, `ACTIVE`, `IDENTITY`, `MOVE` — 어느 슬롯 유형에 배정 가능한지 |
| `SkillType` | ENUM | DATA | `INSTANT`, `DIRECTIONAL`, `DASH`, `GROUND_TARGET`, `PARRY` |
| `HitShape` | ENUM | DATA | `CIRCLE`, `SECTOR`, `RECTANGLE`, `LINE` |
| `HitRange` | FLOAT | DATA | 판정 거리/반경 |
| `HitAngle` | FLOAT | DATA | 부채꼴 판정 시 각도 (0이면 미사용) |
| `SkillCoeff` | FLOAT | DATA | 데미지 계수 |
| `SkillStaggerCoeff` | FLOAT | DATA | 무력화 계수 |
| `SkillGaugeCoeff` | FLOAT | DATA | 아이덴티티 게이지 계수 |
| `SkillThreatBase` | FLOAT | DATA | 기본 위협도 추가량 |
| `BaseCooldown` | FLOAT | DATA | 기본 쿨다운(초) |
| `BaseResourceCost` | FLOAT | DATA | 기본 자원 소모량 |
| `IdentityCost` | FLOAT | DATA | 아이덴티티 발동 시 소모량 (IDENTITY 슬롯만 유효, 나머지 0) |
| `CastTime` | FLOAT | DATA | 시전 시간(초, 즉발 = 0) |
| `CounterTag` | VARCHAR | DATA | 카운터 가능 패턴 태그 (쉼표 구분, 조회 조건 아님) |
| `CancelGrade` | ENUM | DATA | `NONE`, `LIGHT`, `HEAVY`, `CRUSH` |
| `StatusBuildUpPerHit` | FLOAT | DATA | 상태이상 축적량 (확장용, 기본 0) |
| `AnimKey` | VARCHAR | META | 애니메이션 키 |
| `BalanceVersion` | INT | META | 밸런스 버전 |

- PK: `SkillId`
- Unique: `SkillCode + BalanceVersion`

---

### `WeaponDefaultSkillTable` — 무기 타입별 초기 지급 스킬

> WeaponDefaultSkillTable은 무기 타입이 캐릭터 생성·무기 장착 시 자동으로 지급하는 스킬 4종과 이동기/아이덴티티를 정의하며, 초기 해금 목록 생성과 기본 프리셋 생성의 소스가 된다.

**설계 의도:** 이 테이블은 무기 타입의 정적 기본값이다. 플레이어의 현재 장착 상태와 무관하다. 기존 `SkillSetTable`을 대체하며, 슬롯별 단일 FK 구조는 "기본 지급 스킬 1종"을 의미하는 것으로 역할을 명확히 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `WeaponType` | ENUM | PK | `SWORD_SHIELD`, `GREAT_SWORD`, `STAFF` |
| `BasicAttackSkillId` | INT | FK | → `SkillTable.SkillId` |
| `DefaultQSkillId` | INT | FK | 기본 지급 Q 스킬 → `SkillTable.SkillId` |
| `DefaultESkillId` | INT | FK | 기본 지급 E 스킬 → `SkillTable.SkillId` |
| `DefaultRSkillId` | INT | FK | 기본 지급 R 스킬 → `SkillTable.SkillId` |
| `DefaultFSkillId` | INT | FK | 기본 지급 F 스킬 → `SkillTable.SkillId` |
| `IdentitySkillId` | INT | FK | → `SkillTable.SkillId` |
| `MoveSkillId` | INT | FK | → `SkillTable.SkillId` |
| `BalanceVersion` | INT | PK | 밸런스 버전 |

- PK: `WeaponType + BalanceVersion`

---

## 서버 구현으로 넘기는 항목

아래는 런타임 플레이어 상태를 저장하는 DB 테이블이다. 기획 문서 범위 밖이며, 서버 구현 문서에서 정의한다.

| 필요한 테이블 | 이유 |
|---|---|
| 캐릭터별 스킬 해금 이력 | 스킬북 드롭·마을 훈련으로 해금한 스킬 목록 — 플레이어 상태 |
| 캐릭터별 무기×슬롯 배정 | 현재 Q/E/R/F에 어떤 스킬이 있는지 — 플레이어 상태 |
| 캐릭터별 트라이포드 선택 | 어떤 변형 옵션을 골랐는지 — 플레이어 상태 |

서버 구현 제약 사항 (기획 의도):
- 슬롯 배정 가능 스킬은 `SkillTable.WeaponType + SlotCategory`로 필터링 가능해야 한다.
- 해금되지 않은 스킬은 슬롯에 배정할 수 없다.
- 던전 입장 시 현재 슬롯 배정과 해금 목록을 잠근다.

---

### `SkillLevelTable` — 레벨별 스킬 계수 보정

> SkillLevelTable은 스킬 포인트 투자로 올라가는 레벨(1~5)에 따른 계수 배율을 저장하며, 서버가 스킬 발동 시 현재 스킬 레벨로 실제 수치를 산출할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `SkillId` | INT | PK | → `SkillTable.SkillId` |
| `SkillLevel` | INT | PK | 1~5 |
| `CoeffMult` | FLOAT | DATA | 계수 배율 (레벨 1 = 1.00, 레벨 5 = 1.30) |
| `StaggerMult` | FLOAT | DATA | 무력화 배율 |
| `CooldownMult` | FLOAT | DATA | 쿨다운 배율 (레벨업 시 감소) |
| `ResourceCostMult` | FLOAT | DATA | 자원 소모 배율 |

- PK: `SkillId + SkillLevel`
- FK: `SkillId → SkillTable.SkillId`
- Check: `SkillLevel BETWEEN 1 AND 5`

---

### `SkillTripoTable` — 스킬 변형(트라이포드) 옵션

> SkillTripoTable은 스킬 레벨 3/5 해금 시 A/B 중 선택하는 변형 옵션을 저장하며, 캐릭터의 트라이포드 선택에 따라 스킬 수치와 효과를 다르게 적용할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `TripoId` | INT | PK | |
| `SkillId` | INT | FK | → `SkillTable.SkillId` |
| `TripoTier` | INT | DATA | 변형 단계 (1 = 레벨3 해금, 2 = 레벨5 해금) |
| `TripoOption` | ENUM | DATA | `OPTION_A`, `OPTION_B` |
| `TripoDesc` | VARCHAR | DATA | 변형 설명 텍스트 |
| `CoeffDelta` | FLOAT | DATA | 계수 변화량 |
| `CooldownDelta` | FLOAT | DATA | 쿨다운 변화량 |
| `AddEffect` | VARCHAR | DATA | 추가 효과 태그 (쉼표 구분, 예: `ADD_KNOCKBACK,ADD_SLOW`) |

- PK: `TripoId`
- Unique: `SkillId + TripoTier + TripoOption`
- FK: `SkillId → SkillTable.SkillId`

## v1 스킬 목록 초안

각 무기 타입 7개, 총 21개.

| 무기 | 슬롯 | 스킬명 | 판정 타입 | SkillCoeff | 비고 |
|---|---|---|---|---|---|
| 검방 | 기본 | 방패 연계 | DIRECTIONAL | 0.72 | 3타 연계, 마지막 타 0.90 |
| 검방 | Q | 방패 강타 | RECTANGLE | 1.28 | 무력화 집중 |
| 검방 | E | 돌진 베기 | DASH | 1.20 | 이동기 겸용 |
| 검방 | R | 도발 보호막 | INSTANT | 0.00 | 유틸, 위협도 +400 |
| 검방 | F | 방패 내려치기 | SECTOR | 1.70 | CancelGrade=HEAVY |
| 검방 | 아이덴티티 | 철벽 태세 | INSTANT | 2.02 | 방어 강화 + 패링 보상 증가 |
| 검방 | 이동기 | 방패 압박 | DASH | 0.00 | 이동 전용 |
| 대검 | 기본 | 대검 연계 | DIRECTIONAL | 0.68 | 2타 연계, 느리고 넓음 |
| 대검 | Q | 전방 대베기 | SECTOR | 1.40 | 광범위 잡몹 처리 |
| 대검 | E | 전진 참격 | DASH | 1.28 | 추격 이동기 |
| 대검 | R | 차지 강공격 | RECTANGLE | 1.98 | 무력화 압박 핵심 |
| 대검 | F | 내려찍기 | CIRCLE | 1.70 | CancelGrade=HEAVY, 슈퍼아머 |
| 대검 | 아이덴티티 | 폭딜 집중 | DIRECTIONAL | 2.42 | 고위력 일점집중 |
| 대검 | 이동기 | 도약 베기 | DASH | 0.00 | 이동 전용 |
| 지팡이 | 기본 | 마법 투사 | LINE | 0.64 | 자동 유도 원거리 |
| 지팡이 | Q | 관통 마법 | LINE | 1.34 | 직선 관통 |
| 지팡이 | E | 폭발 장판 | GROUND_TARGET | 1.36 | 지정 위치 폭발 |
| 지팡이 | R | 속성 디버프 | CIRCLE | 0.80 | 상태이상 축적 |
| 지팡이 | F | 대주문 | SECTOR | 1.82 | 긴 시전, CancelGrade=CRUSH |
| 지팡이 | 아이덴티티 | 마력 폭주 | CIRCLE | 2.26 | 광역 제어 |
| 지팡이 | 이동기 | 마력 도약 | DASH | 0.00 | 이동 전용 |

## 제약과 예외

- SkillCoeff 수치는 플레이 테스트 전 초안 값이다. 밸런스 버전 관리로 추적한다.
- 실제 행 데이터는 `data/skills/` 경로의 JSON 파일에서 관리한다.
- 판정 처리 세부 구현(히트박스 계산 등)은 서버 구현 문서에서 확정한다.

## 관련 문서

- [../common/skill.md](../common/skill.md)
- [ingame/combat_formulas.md](./combat_formulas.md)
- [ingame/combat_data_tables.md](./combat_data_tables.md)
