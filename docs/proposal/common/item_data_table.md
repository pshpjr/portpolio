# 아이템 / 드롭 데이터 테이블 구조

## 목적

아이템, 드롭, 전리품 선호 보정, 재련 비용 authoring에 필요한 테이블 구조와 책임 범위를 정의한다.
실제 행 데이터는 JSON/CSV로 관리하고, 이 문서는 컬럼 계약과 테이블 간 연결 규칙만 다룬다.

---

## 명명 규칙

| 구분 | 접두사 | 예시 | 설명 |
|---|---|---|---|
| 기획 데이터 테이블 | 없음 | `Item_Table`, `Reward_Table` | JSON/CSV로 관리하는 authoring 데이터. 정적 규칙 중심 |
| DB 테이블 | `T_` | `T_Item`, `T_Account` | 서버가 런타임에 읽고 쓰는 DB 테이블. 인스턴스 상태 포함 |

---

## 테이블 전체 관계 다이어그램

```
Item_Table.item_id
  → Reward_Table.item_id            (드롭 가중치 조회)
  → ReforgeCostTable (rarity 기준)  (재련 비용 조회)
  → BaseStatSetTable.base_stat_set_id (기본 스탯 조회)
  → OptionPoolTable.option_pool_id  (옵션 풀 조회)

Item_Weapon_Table
  앞부분 컬럼 = Item_Table 공통 컬럼과 동일 (상속 구조)
  뒷부분 컬럼 = 무기 전용 전투 메타

Dungeon_Table.purification_reward_id → Reward_Table.reward_id (그룹 키)
Dungeon_Table.scouting_reward_id     → Reward_Table.reward_id (그룹 키)

T_Item.item_id → Item_Table.item_id (또는 Item_Weapon_Table.item_id)
  현재 bind_state, trade_count 등 런타임 상태 보관
```

---

## 기획 데이터 테이블 정의

### `Item_Table` — 아이템 공통 메타 (기본 틀)

> Item_Table은 모든 아이템 종류의 공통 메타(이름, 등급, 슬롯, 귀속 규칙, 판매 규칙)를 저장하는 기본 틀이다.
> 무기가 아닌 아이템(방어구, 장신구, 소모품, 재료, 스킬북)은 이 테이블만 사용한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `item_id` | INT | PK | |
| `item_code` | VARCHAR | DATA | 내부 코드명 |
| `item_name` | VARCHAR | DATA | 표시 이름 |
| `item_category` | ENUM | DATA | `WEAPON`, `ARMOR`, `ACCESSORY`, `CONSUMABLE`, `MATERIAL`, `SKILL_BOOK` |
| `rarity` | ENUM | DATA | `NORMAL`, `UNCOMMON`, `RARE`, `EPIC` |
| `equip_slot` | ENUM | DATA | `WEAPON`, `CHEST`, `LEGS`, `HANDS`, `NECK`, `RING_L`, `RING_R`, NULL |
| `base_stat_set_id` | INT | FK | → `BaseStatSetTable.base_stat_set_id` |
| `option_pool_id` | INT | FK | → `OptionPoolTable.option_pool_id` |
| `sell_value` | INT | DATA | NPC 판매 가치 (0 = 판매 불가) |
| `bind_rule` | ENUM | DATA | `NONE`, `BIND_ON_ACQUIRE`, `BIND_ON_EQUIP` |
| `icon_key` | VARCHAR | META | 아이콘 리소스 키 |

- PK: `item_id`
- `bind_rule` 의미:
  - `NONE` — 귀속 없음 (상점 구매 소모품 등)
  - `BIND_ON_ACQUIRE` — 획득 즉시 계정 귀속 (v1 드롭 아이템 기본값)
  - `BIND_ON_EQUIP` — 장착 시 계정 귀속 (v2 이후 확장용)

---

### `Item_Weapon_Table` — 무기 아이템 (공통 + 전투 전용 상속 구조)

> Item_Weapon_Table은 Item_Table의 공통 컬럼을 앞부분에 그대로 포함하고,
> 그 뒤에 무기 전투 전용 메타를 붙인 단일 테이블이다.
> C++ `struct ItemWeapon : ItemBase {}` 처럼 앞부분 레이아웃이 Item_Table과 일치한다.
> 무기 아이템은 이 테이블만 사용하며 Item_Table에 별도 행을 두지 않는다.

**앞부분 — Item_Table과 동일한 공통 컬럼 (순서·타입 일치 필수):**

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `item_id` | INT | PK | |
| `item_code` | VARCHAR | DATA | 내부 코드명 |
| `item_name` | VARCHAR | DATA | 표시 이름 |
| `item_category` | ENUM | DATA | 항상 `WEAPON` |
| `rarity` | ENUM | DATA | `NORMAL`, `UNCOMMON`, `RARE`, `EPIC` |
| `equip_slot` | ENUM | DATA | 항상 `WEAPON` |
| `base_stat_set_id` | INT | FK | → `BaseStatSetTable.base_stat_set_id` |
| `option_pool_id` | INT | FK | → `OptionPoolTable.option_pool_id` |
| `sell_value` | INT | DATA | NPC 판매 가치 |
| `bind_rule` | ENUM | DATA | `NONE`, `BIND_ON_ACQUIRE`, `BIND_ON_EQUIP` |
| `icon_key` | VARCHAR | META | 아이콘 리소스 키 |

**뒷부분 — 무기 전투 전용 추가 컬럼:**

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `weapon_type` | ENUM | DATA | `SWORD_SHIELD`, `GREAT_SWORD`, `STAFF` |
| `base_atk` | FLOAT | DATA | 무기 기본 공격력 |
| `atk_speed` | FLOAT | DATA | 공격 속도 배율 |
| `stagger_add` | INT | DATA | 무력화 게이지 기본 기여량 |
| `parry_window_bonus` | FLOAT | DATA | 패링 판정 보정 (무기 고유) |
| `animation_set_id` | INT | DATA | 모션 세트 ID |
| `balance_version` | INT | META | 밸런스 버전 |

- PK: `item_id`
- 앞부분 컬럼 순서와 타입은 Item_Table과 반드시 일치해야 한다. 공통 컬럼 변경 시 두 테이블을 동기 수정한다.

---

### `BaseStatSetTable` — 기본 스탯 묶음

> BaseStatSetTable은 아이템 종류별 기본 스탯 집합을 저장하며, 장착 시 플레이어 스탯 계산에서 단일 조회로 기본 수치를 더할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `base_stat_set_id` | INT | PK | |
| `stat_profile_type` | ENUM | DATA | `WEAPON`, `ARMOR`, `ACCESSORY`, `CONSUMABLE` |
| `attack_power` | FLOAT | DATA | 공격력 |
| `weapon_power` | FLOAT | DATA | 무기 공격력 |
| `defense` | FLOAT | DATA | 방어력 |
| `max_hp` | FLOAT | DATA | 최대 HP |
| `critical_chance` | FLOAT | DATA | 치명타 확률 |
| `critical_damage` | FLOAT | DATA | 치명타 추가 피해 |
| `cooldown_reduction` | FLOAT | DATA | 쿨다운 감소 |
| `parry_window_bonus` | FLOAT | DATA | 패링 판정 보정 |
| `move_speed_bonus` | FLOAT | DATA | 이동 속도 보정 |
| `balance_version` | INT | META | 밸런스 버전 |

- PK: `base_stat_set_id`

---

### `OptionPoolTable` — 랜덤 옵션 풀

> OptionPoolTable은 아이템 등급별 랜덤 옵션 후보군을 저장하며, 드롭·재련 시 옵션 재굴림에서 풀 단위로 참조할 수 있게 한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `option_pool_id` | INT | PK | |
| `pool_name` | VARCHAR | DATA | 옵션 풀 이름 |
| `item_category` | ENUM | DATA | 적용 아이템 분류 |
| `option_tag` | VARCHAR | DATA | 옵션 종류 태그 |
| `min_value` | FLOAT | DATA | 최솟값 |
| `max_value` | FLOAT | DATA | 최댓값 |
| `weight` | FLOAT | DATA | 가중치 |
| `balance_version` | INT | META | 밸런스 버전 |

- PK: `option_pool_id`

---

### `Reward_Table` — 던전 보상 드롭 정의 (멀티맵 구조)

> Reward_Table은 보상 그룹별 드롭 항목을 저장한다.
> `reward_id`는 고유 PK가 아닌 그룹 키(멀티맵)이며, 동일 `reward_id`에 여러 행이 존재할 수 있다.
> 서버는 `reward_id`로 해당 그룹의 전체 행을 조회해 드롭 후보 목록을 구성한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `row_id` | INT | PK | 행 고유 식별자 (auto-increment) |
| `reward_id` | INT | DATA | 보상 그룹 키 (멀티맵 — 비고유) |
| `item_id` | INT | FK | → `Item_Table.item_id` 또는 `Item_Weapon_Table.item_id` |
| `source_type` | ENUM | DATA | `NORMAL_MONSTER`, `ELITE`, `BOSS_CHEST`, `GIMMICK_BONUS` |
| `source_id` | INT | DATA | 몬스터/상자/기믹 ID |
| `base_weight` | FLOAT | DATA | 기본 가중치 |
| `weapon_type_filter` | ENUM | DATA | 전리품 선호 무기 타입 필터 (`SWORD_SHIELD`, `GREAT_SWORD`, `STAFF`, `ANY`) |
| `weight_multiplier_on_match` | FLOAT | DATA | 선호 무기 일치 시 가중치 배율 |
| `min_count` | INT | DATA | 최소 수량 |
| `max_count` | INT | DATA | 최대 수량 |
| `allow_rare_or_above` | BOOL | DATA | 희귀 이상 등급 드롭 허용 여부 |
| `balance_version` | INT | META | 밸런스 버전 |

- PK: `row_id`
- `reward_id`는 **비고유** — 동일 값으로 여러 행이 존재한다 (보상 그룹 멀티맵)

---

### `Dungeon_Table` — 던전 기본 정보 + 보상 링크

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `dungeon_id` | INT | PK | |
| `dungeon_code` | VARCHAR | DATA | 내부 코드명 |
| `dungeon_name` | VARCHAR | DATA | 표시 이름 |
| `purification_reward_id` | INT | FK | 정화 원정 보상 그룹 → `Reward_Table.reward_id` |
| `scouting_reward_id` | INT | FK | 정찰 원정 보상 그룹 → `Reward_Table.reward_id` |
| `max_party_size` | INT | DATA | 최대 파티 인원 |
| `balance_version` | INT | META | 밸런스 버전 |

- PK: `dungeon_id`

---

### `ReforgeCostTable` — 재련 비용

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `rarity` | ENUM | PK | 아이템 등급 |
| `attempt_count` | INT | PK | 동일 아이템 누적 재련 횟수 |
| `gold_cost` | INT | DATA | 골드 비용 |
| `material_cost_id` | INT | DATA | 추가 재화 ID (NULL = 골드만) |

- PK: `rarity + attempt_count`
- 귀속 전환은 `Item_Table.bind_rule`이 `BIND_ON_ACQUIRE`이면 획득 시 이미 귀속이므로 재련 시 추가 전환 불필요.

---

## DB 테이블 정의 (`T_` 접두사)

### `T_Item` — 아이템 인스턴스 (런타임 상태)

> T_Item은 드롭·구매로 생성된 개별 아이템 인스턴스의 현재 상태를 저장한다.
> 귀속 상태, 거래 잔여 횟수, 내구도, 재련 횟수 같은 **런타임 상태**가 여기에 있다.
> 기획 테이블(`Item_Table`, `Item_Weapon_Table`)은 정적 규칙만 담고 인스턴스 상태는 이 테이블에서 관리한다.

| 컬럼 | 타입 | 역할 | 설명 |
|---|---|---|---|
| `instance_id` | BIGINT | PK | 아이템 인스턴스 고유 ID |
| `item_id` | INT | FK | → `Item_Table.item_id` 또는 `Item_Weapon_Table.item_id` |
| `owner_account_id` | BIGINT | FK | 소유 계정 ID |
| `owner_char_id` | BIGINT | FK | 소유 캐릭터 ID (NULL = 공유 창고에 있음) |
| `bind_state` | ENUM | DATA | `UNBOUND`, `ACCOUNT_BOUND` |
| `trade_count` | INT | DATA | 남은 거래 가능 횟수 (v1은 항상 0) |
| `durability` | INT | DATA | 현재 내구도 |
| `reforge_count` | INT | DATA | 누적 재련 횟수 |
| `created_at` | TIMESTAMP | META | 생성 시각 |

- PK: `instance_id`
- UI 표시용: `bind_state` → 귀속 상태 텍스트, `trade_count` → 거래 횟수 표시

---

## UI 텍스트 관리 (`UI_Text_Table`)

> UI에 표시되는 모든 문자열은 `UI_Text_Table`(string_key → localized text)에서 관리한다.
> 클라이언트 코드에서 game state → string_key 매핑을 하드코딩한다.
> 매핑 자체는 변경될 일이 없으므로 테이블화하지 않는다.

**아이템 UI에서 사용하는 string_key 예시:**

| string_key | 용도 |
|---|---|
| `item.bind.account_bound` | 귀속 상태: 계정 귀속 표시 |
| `item.bind.unbound` | 귀속 상태: 귀속 없음 표시 |
| `item.trade.unavailable` | 거래 횟수: 거래 불가 표시 (v1) |
| `item.trade.count` | 거래 횟수: `{n}회 거래 가능` 형식 (v2 이후) |

- `UI_Text_Table` 정의(컬럼 계약, 실제 텍스트 행)는 클라이언트 리소스 문서에서 관리한다.
- 별도 UI 표현 계약 테이블(`ItemUiStateTable` 등)은 만들지 않는다.

---

## 테이블 연결 규칙

```
던전 입장 + 장착 무기 (= 전리품 선호 자동 결정)
  → Dungeon_Table (dungeon_id + reward_mode)
    → purification_reward_id 또는 scouting_reward_id
      → Reward_Table.reward_id 그룹 전체 행 조회
        → weapon_type_filter 로 선호 무기 bias 적용
        → base_weight × weight_multiplier_on_match 로 최종 가중치 계산

아이템 드롭 확정
  → Item_Table.item_id 또는 Item_Weapon_Table.item_id (정적 규칙 조회)
  → T_Item 신규 행 생성 (인스턴스 상태 초기화: bind_state = bind_rule 기준 결정)
    → BaseStatSetTable (기본 스탯)
    → OptionPoolTable (랜덤 옵션 재굴림)

아이템 UI 렌더링
  → T_Item.bind_state → string_key → UI_Text_Table (귀속 상태 텍스트)
  → T_Item.trade_count → string_key → UI_Text_Table (거래 횟수 표시)

재련 요청
  → ReforgeCostTable (rarity + attempt_count)
    → gold_cost, material_cost_id 확인
  → T_Item.reforge_count 증가
```

---

## 제약과 예외

- 실제 드롭 확률 수치는 플레이 로그와 파밍 속도 검증 후 조정한다.
- `Item_Weapon_Table`의 앞부분 컬럼 순서와 타입은 `Item_Table`과 반드시 일치해야 한다.
- `Reward_Table`의 `reward_id`는 비고유이므로 단독 FK JOIN 시 복수 행이 반환된다. 서버는 이를 의도된 멀티맵으로 처리해야 한다.
- v1에서 `T_Item.trade_count`는 항상 0이다. 거래 추가 시 `Item_Table.bind_rule`에 `TRADEABLE_N_TIMES` Enum을 추가하고 초기값을 채운다.
- v1에서 `T_Item.bind_state`의 초기값은 `bind_rule = BIND_ON_ACQUIRE`이면 `ACCOUNT_BOUND`, `NONE`이면 `UNBOUND`로 설정한다.
- 저장 구조와 패킷 구조는 서버 구현 문서에서 확정한다.

---

## 관련 문서

- [item.md](./item.md)
- [skill.md](./skill.md)
- [data-table-conventions.md](./data-table-conventions.md)
- [../outgame/economy.md](../outgame/economy.md)
- [../ingame/dungeon_progress.md](../ingame/dungeon_progress.md)
- [../ingame/combat_data_tables.md](../ingame/combat_data_tables.md)
