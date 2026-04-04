# 데이터 테이블 문서 규칙

## 목적

기획서 내 모든 데이터 테이블 문서의 작성 형식을 통일하고, 테이블 구조 설계 판단 기준을 정의한다.
이 문서는 `design-author` 에이전트와 `data-table-reviewer` 에이전트가 공통 참조하는 단일 소스다.

---

## 기획 테이블 vs 서버 DB 테이블 경계

기획 데이터 테이블 문서는 **디자이너가 authoring하는 정적 정의 테이블**만 다룬다.

| 기획 문서 범위 (이 문서 대상) | 서버 구현 문서 범위 |
|---|---|
| 스킬 정의, 계수, 판정 형태 | 캐릭터가 해금한 스킬 목록 |
| 무기 타입별 기본 지급 스킬 | 캐릭터의 현재 슬롯 배정 |
| 아이템 템플릿, 등급, 옵션 풀 | 캐릭터 인벤토리, 장착 상태 |
| 드롭 테이블, 가중치 | 드롭 이력, 보상 수령 기록 |
| 몬스터/보스 패턴 정의 | 현재 인던의 몬스터 인스턴스 상태 |

**경계 판단 기준**: 행의 주체가 **콘텐츠 정의**(스킬, 아이템, 몬스터)이면 기획 테이블. 행의 주체가 **플레이어/캐릭터/세션**이면 서버 DB 테이블.

기획 문서에서 서버 DB 테이블이 필요하다고 판단되면, 해당 문서의 "서버 구현으로 넘기는 항목" 섹션에 테이블 목적과 기획 의도만 기술하고 상세 스키마는 서버 구현 문서에 위임한다.

---

## 테이블 문서 표준 구조

모든 데이터 테이블 문서는 아래 섹션 순서를 따른다.

```
1. 목적
2. 테이블 전체 관계 다이어그램
3. 각 테이블 정의 (테이블마다 반복)
   3a. 의도 (한 문장 — 이 테이블이 없으면 무엇이 불가능한가)
   3b. 컬럼 표 (PK/FK/Type/설명)
   3c. PK / FK 선언
4. 테이블 연결 규칙 (런타임에서 어떤 키로 어떤 테이블을 조인하는지)
5. 제약과 예외
6. 관련 문서
```

---

## 테이블 의도 섹션 작성 규칙

각 테이블 정의 앞에 **한 문장**으로 의도를 적는다.

형식:
```
> [테이블명]은 [어떤 주체]의 [어떤 상태/관계]를 저장하며, [어떤 런타임 조회]를 가능하게 한다.
```

예시:
```
> WeaponLevelStatLinkTable은 무기 종류와 강화 단계를 WeaponStatTableId에 연결하며,
> 무기 장착/강화 시점에 단일 키 조회로 전투 스탯 스냅샷을 가져올 수 있게 한다.
```

의도가 한 문장으로 쓰이지 않는다면 테이블 역할이 복수일 가능성 — 분리를 검토한다.

---

## 컬럼 표 형식

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `ColumnName` | INT / VARCHAR / ENUM / FLOAT / BOOL | PK / FK / DATA / META | 설명 |

- **역할** 열은 `PK`, `FK`, `DATA`(순수 데이터), `META`(버전/플래그 등 운영용) 중 하나
- 타입은 구현 언어 종속 없이 논리 타입으로 기재 (INT, VARCHAR, FLOAT, BOOL, ENUM, JSON)
- FK 컬럼은 설명란에 `→ [참조테이블.참조컬럼]` 형식으로 명시

---

## PK / FK 선언 형식

컬럼 표 아래에 별도 블록으로 선언한다.

```
- PK: `ColumnA`
- PK: `ColumnA + ColumnB`  (복합 PK)
- FK: `ColumnX → TableY.ColumnY`
- Unique: `ColumnA + ColumnB`
- Check: `ColumnA BETWEEN 1 AND 40`
```

---

## 복합 PK 사용 기준

아래 조건 중 하나라도 해당하면 복합 PK를 사용한다.

| 조건 | 예시 |
|---|---|
| 두 외래 키의 조합이 유일한 행을 결정하는 매핑 테이블 | `WeaponType + EnhanceLevel → WeaponStatTableId` |
| 자연 식별자가 2개 이상이고 대리 키(surrogate key) 생성이 불필요한 경우 | `Level + BalanceVersion` |
| 동일 엔티티의 버전 이력을 관리하는 경우 | `SkillId + BalanceVersion` |

단일 정수 대리 키(`Id`)를 쓰는 경우:
- 행 수가 많아 외부 참조가 빈번하거나
- 자연 식별자가 변경될 가능성이 있거나
- 복합 PK가 FK로 전파되어 참조 테이블이 많아지는 경우

---

## 배열 컬럼 vs 별도 테이블 분리 기준

| 상황 | 권장 방식 |
|---|---|
| 값이 **조회 조건**으로 쓰이지 않고 문자열 태그 나열만 필요 | 쉼표 구분 VARCHAR (예: `CounterTag TEXT`) |
| 값 개수가 **고정**되고 의미 있는 슬롯 이름이 있다 | 별도 컬럼 (예: `QSkillId`, `ESkillId`) |
| 값 개수가 **가변**이고 각 항목을 **독립 참조/쿼리**해야 한다 | 별도 1:N 테이블 |
| 값 항목 자체가 성장/변경되는 엔티티다 | 반드시 별도 테이블 |

배열처럼 보이는 컬럼 예시와 판정:
- `CounterTag TEXT` — "PARRY,DODGE" 형태, 조회 조건 없음 → 쉼표 구분 허용
- `QSkillId, ESkillId, RSkillId, FSkillId` — 고정 4슬롯, 각 슬롯 의미 다름 → 별도 컬럼 허용
- 플레이어가 장착한 스킬 목록 — 가변, 각 항목을 독립 참조 → 별도 테이블 필수

---

## 정규화 판단 기준

아래 징후가 보이면 분리를 검토한다.

1. **같은 값 반복**: 여러 행에 동일한 값 묶음이 반복되면 별도 테이블 + FK
2. **두 가지 개념 혼재**: 템플릿(정적 정의)과 인스턴스(런타임 상태)가 같은 테이블에 있으면 분리
3. **업데이트 이상**: 한 개념이 바뀔 때 여러 행을 동시에 고쳐야 하면 분리 신호
4. **타입 혼재**: 한 컬럼이 nullable이면서 특정 조건에서만 의미 있으면 별도 테이블 검토

자주 나타나는 분리 패턴:
- `WeaponTable` (무기 메타) vs `WeaponStatTable` (전투 수치) — 역할 분리
- `SkillSetTable` (무기 기본 템플릿) vs `CharacterSkillPresetTable` (플레이어 장착 상태) — 정적 정의 vs 런타임 상태 분리
- `ItemTemplateTable` (아이템 정의) vs `CharacterInventoryTable` (소유 인스턴스) — 정의 vs 소유 분리

---

## BalanceVersion 컬럼 사용 기준

| 테이블 성격 | BalanceVersion 포함 여부 |
|---|---|
| 밸런스 수치가 포함된 스탯/계수 테이블 | 필수 |
| 메타/템플릿 테이블 (이름, 코드, 아이콘 등) | 선택 (수치 포함 시 필수) |
| 런타임 인스턴스 테이블 (플레이어 소유, 장착 상태) | 불필요 |
| 링크/매핑 테이블 (두 테이블 연결만) | 참조 테이블이 BalanceVersion을 가지면 FK로 전파 |

---

## 테이블 관계 다이어그램 형식

텍스트 다이어그램을 사용한다. 화살표 방향은 FK → 참조 대상.

```
Player.Level
  → UserLevelStatLinkTable.Level (+ BalanceVersion)
    → PlayerStatTable.PlayerStatTableId

Weapon.WeaponType + Weapon.EnhanceLevel
  → WeaponLevelStatLinkTable (+ BalanceVersion)
    → WeaponStatTable.WeaponStatTableId
```

---

## 관련 문서

- [proposal/AGENTS.md](../AGENTS.md)
- [design-author 에이전트](../../../.claude/agents/design-author.md)
- [data-table-reviewer 에이전트](../../../.claude/agents/data-table-reviewer.md)
