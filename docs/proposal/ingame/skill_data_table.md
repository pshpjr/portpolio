# 스킬 데이터 테이블 구조

## 목적

스킬 정의 데이터의 테이블 구조와 책임 범위를 정의한다. 실제 행 데이터는 별도 JSON에서 관리한다.

## 핵심 규칙

- 스킬은 무기 타입 단위로 `SkillSetTable`에 묶인다.
- 각 스킬의 계수와 판정 데이터는 `SkillTable`이 담당한다.
- 레벨별 성장 보정은 `SkillLevelTable`이 담당한다.
- 스킬 변형(트라이포드) 옵션은 `SkillTripoTable`이 담당한다.

## 테이블 구조

### `SkillTable` - 스킬 기본 정의

| 컬럼 | 설명 |
|---|---|
| `SkillId` | PK |
| `SkillCode` | 스킬 코드명 (예: `SWORD_SHIELD_Q`) |
| `SkillName` | 표시 이름 |
| `WeaponType` | 무기 타입 (`SWORD_SHIELD`, `GREAT_SWORD`, `STAFF`) |
| `SkillSlot` | 슬롯 분류 (`BASIC_ATTACK`, `ACTIVE_Q`, `ACTIVE_E`, `ACTIVE_R`, `ACTIVE_F`, `IDENTITY`, `MOVE`) |
| `SkillType` | 판정 타입 (`INSTANT`, `DIRECTIONAL`, `DASH`, `GROUND_TARGET`, `PARRY`) |
| `HitShape` | 판정 형태 (`CIRCLE`, `SECTOR`, `RECTANGLE`, `LINE`) |
| `HitRange` | 판정 거리/반경 (float) |
| `HitAngle` | 부채꼴 판정 시 각도 (float) |
| `SkillCoeff` | 데미지 계수 |
| `SkillStaggerCoeff` | 무력화 계수 |
| `SkillGaugeCoeff` | 아이덴티티 게이지 계수 |
| `SkillThreatBase` | 기본 위협도 추가량 |
| `BaseCooldown` | 기본 쿨다운(초) |
| `BaseResourceCost` | 기본 자원 소모량 |
| `IdentityCost` | 아이덴티티 발동 시 소모량 (아이덴티티 스킬만) |
| `CastTime` | 시전 시간(초, 즉발 = 0) |
| `CounterTag` | 카운터 가능 패턴 태그 (쉼표 구분) |
| `CancelGrade` | 캔슬 등급 (`NONE`, `LIGHT`, `HEAVY`, `CRUSH`) |
| `StatusBuildUpPerHit` | 상태이상 축적량 (확장용, 기본 0) |
| `AnimKey` | 애니메이션 키 |
| `BalanceVersion` | 밸런스 버전 |

### `SkillSetTable` - 무기 타입별 스킬 세트 묶음

| 컬럼 | 설명 |
|---|---|
| `SkillSetId` | PK |
| `WeaponType` | 무기 타입 |
| `BasicAttackSkillId` | FK → SkillTable |
| `QSkillId` | FK → SkillTable |
| `ESkillId` | FK → SkillTable |
| `RSkillId` | FK → SkillTable |
| `FSkillId` | FK → SkillTable |
| `IdentitySkillId` | FK → SkillTable |
| `MoveSkillId` | FK → SkillTable |
| `BalanceVersion` | 밸런스 버전 |

### `SkillLevelTable` - 레벨별 스킬 계수 보정

| 컬럼 | 설명 |
|---|---|
| `SkillLevelId` | PK |
| `SkillId` | FK → SkillTable |
| `SkillLevel` | 1~5 |
| `CoeffMult` | 계수 배율 (예: 레벨 1 = 1.00, 레벨 5 = 1.30) |
| `StaggerMult` | 무력화 배율 |
| `CooldownMult` | 쿨다운 배율 (레벨업 시 감소) |
| `ResourceCostMult` | 자원 소모 배율 |

### `SkillTripoTable` - 스킬 변형(트라이포드) 옵션

| 컬럼 | 설명 |
|---|---|
| `TripoId` | PK |
| `SkillId` | FK → SkillTable |
| `TripoTier` | 변형 단계 (1 = 레벨3 해금, 2 = 레벨5 해금) |
| `TripoOption` | A/B 중 선택 (`OPTION_A`, `OPTION_B`) |
| `TripoDesc` | 변형 설명 텍스트 |
| `CoeffDelta` | 계수 변화량 |
| `CooldownDelta` | 쿨다운 변화량 |
| `AddEffect` | 추가 효과 태그 (예: `ADD_KNOCKBACK`, `ADD_SLOW`) |

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
