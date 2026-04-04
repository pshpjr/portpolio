# Office-Hours Proposal and Data Table Review

- Generated: 2026-04-04
- Scope: `main.md`, `pending-decisions.md`, `common/data-table-conventions.md`, `common/item_data_table.md`, `ingame/combat_data_tables.md`, `ingame/encounter_data_tables.md`, `ingame/skill_data_table.md`
- Perspectives: planning, server, client

## Round Verdict

기획 방향 자체는 유지 가능하다.
다만 현재 데이터 테이블 문서는 "구현 착수 직전 계약"으로 보기에는 아직 이르다.
특히 보상 모드/전리품 선호 연결, 무기 authoring 소유권, HUD/피드백 key, 슬롯/프리셋 제약이 문서 간에 충분히 고정되지 않았다.

## Highest-Priority Findings

### 1. 전리품 선호 보정과 보상 모드 계약이 아직 닫혀 있지 않다

- 대상 문서:
  - `docs/proposal/common/item_data_table.md`
  - `docs/proposal/outgame/dungeon_entry.md`
  - `docs/proposal/ingame/dungeon_progress.md`
- 문제:
  - `LootPreferenceBiasTable`은 `BiasProfileId`를 PK로 가지지만, 어떤 던전/보상 모드/드롭 소스가 이 프로필을 쓰는지 연결 규칙이 없다.
  - `DropTable.RewardMode`만으로는 `정화/정찰`의 전체 보상 풀, 상위 등급 차단, 추가 보상 상자 조건을 닫기 어렵다.
- 왜 중요한가:
  - 서버는 어떤 bias와 reward rule을 적용해야 하는지 테이블만 보고 결정할 수 없다.
  - 현재 pending decision인 `정화/정찰 2모드 유지` 여부가 테이블 구조에도 바로 영향을 준다.
- 권장 보강:
  - `DungeonRewardProfileTable` 또는 동급의 authoring 테이블을 추가해 `DungeonId + RewardMode`별 보상 정책을 분리한다.
  - `LootPreferenceBiasTable`은 글로벌 규칙으로 유지할지, 던전/보상 프로필이 참조하는 구조로 바꿀지 결정한다.

### 2. 무기 authoring의 단일 소스가 불명확하다

- 대상 문서:
  - `docs/proposal/common/item_data_table.md`
  - `docs/proposal/ingame/combat_data_tables.md`
  - `docs/proposal/common/item.md`
- 문제:
  - `ItemTemplateTable`가 무기 아이템 메타를 담는 반면, `WeaponTable`도 무기 메타와 경제/장비 관련 필드를 함께 가진다.
  - `TradeLimitCount`는 양쪽 문맥에 걸쳐 있고, 내구도/수리 역시 아이템 시스템과 전투 시스템 사이에 걸쳐 있다.
- 왜 중요한가:
  - 서버는 어떤 테이블이 인벤토리/경제의 정본인지, 어떤 테이블이 전투 참조의 정본인지 분명해야 한다.
  - 클라이언트도 아이템 툴팁/장착 UI와 전투 UI가 같은 무기 정보를 다른 경로에서 중복 조립하면 위험하다.
- 권장 보강:
  - `ItemTemplateTable`는 아이템 공통 메타/경제/UI를, `WeaponTable`은 전투 전용 메타를 담당하게 분리하고 둘을 FK로 연결한다.
  - 내구도/수리/거래 횟수처럼 경계에 걸친 필드는 어느 쪽이 정본인지 명시한다.

### 3. 보스 공통 전투 프로필이 패턴 테이블과 분리되어 있지 않다

- 대상 문서:
  - `docs/proposal/ingame/encounter_data_tables.md`
  - `docs/proposal/ingame/boss_pattern.md`
- 문제:
  - `encounter_data_tables.md`에는 패턴/전조/체크포인트만 있고, 실제 서버가 함께 읽어야 하는 `MaxHP`, `StaggerGauge`, `StatusResistance`, `EnrageTime`, 다운 후 보호 규칙은 `boss_pattern.md` 본문에만 남아 있다.
- 왜 중요한가:
  - 지금 구조로는 패턴 authoring과 보스 전투 프로필 authoring이 분리되지 않아, data-driven 설명력이 약하고 서버 로더 경계도 흐려진다.
- 권장 보강:
  - `BossCombatProfileTable`, 필요 시 `BossStaggerRuleTable` 계열을 두어 보스 공통 수치와 회차별 무력화 규칙을 패턴 테이블과 분리한다.

### 4. 클라이언트가 바로 쓸 수 있는 HUD/피드백 key 계약이 부족하다

- 대상 문서:
  - `docs/proposal/ingame/encounter_data_tables.md`
  - `docs/proposal/ingame/combat.md`
  - `docs/proposal/ingame/combat_ui_wireframe.md`
  - `docs/proposal/common/item_data_table.md`
- 문제:
  - `BossTelegraphTable`, `CombatFeedbackTable`, `DungeonGimmickTable`에는 표시 위치와 시간은 있지만, 실제 UI 문구 key, 월드 마커 key, 역할 안내 key, 색상/등급 key가 부족하다.
  - `ItemUiStateTable`도 배지/경고 색상은 있지만, 차단 사유나 툴팁 사유 코드를 안정적으로 묶는 키가 없다.
- 왜 중요한가:
  - 클라이언트는 현재 문서만으로는 전조/경고/차단 사유/UI 배지를 일관되게 렌더링할 수 없다.
- 권장 보강:
  - encounter 쪽에는 `TextKey`, `MarkerKey`, `SeverityTag`, `RoleHintTextKey` 계열을 추가한다.
  - item 쪽에는 `LockReasonTag`, `TooltipReasonKey` 같은 명시적 UI 계약을 추가한다.

### 5. 스킬 슬롯/프리셋 제약이 데이터 계약으로 충분히 닫혀 있지 않다

- 대상 문서:
  - `docs/proposal/ingame/skill_data_table.md`
  - `docs/proposal/common/skill.md`
  - `docs/proposal/common/user.md`
- 문제:
  - `SkillTable.SlotCategory = ACTIVE`만으로는 특정 스킬이 어떤 슬롯에 갈 수 있는지, 기본 슬롯이 무엇인지, UI에서 어떤 식으로 보여야 하는지 애매하다.
  - `CharacterSkillPresetTable`은 서버 구현으로 넘기지만, 기획 단계에서 최소한 어떤 key로 검증할지 문서가 약하다.
- 왜 중요한가:
  - 서버는 슬롯 유효성 검사를, 클라는 슬롯 배치/비활성화 이유 표시를 모두 같은 계약으로 알아야 한다.
- 권장 보강:
  - `AllowedSlotKey`, `DefaultSlotKey`, `UiAimType`, `UnlockSourceTag` 같은 필드를 추가 검토한다.
  - 프리셋 검증 규칙을 "무기 타입 + 슬롯 key + 해금 여부"로 명문화한다.

### 6. 데이터 테이블 문서들이 새 conventions와 완전히 맞물려 있지 않다

- 대상 문서:
  - `docs/proposal/common/item_data_table.md`
  - `docs/proposal/ingame/combat_data_tables.md`
  - `docs/proposal/ingame/encounter_data_tables.md`
- 문제:
  - `common/data-table-conventions.md`가 요구하는 `의도 한 문장`, `타입/역할/설명`, `PK/FK 선언`, `관계 다이어그램` 구조를 일부 문서만 충족한다.
  - 특히 `item_data_table.md`, `encounter_data_tables.md`는 아직 구식 요약 형태에 가깝다.
- 왜 중요한가:
  - 형식이 흔들리면 서버/클라/기획이 같은 테이블을 읽어도 판단 기준이 달라진다.
- 권장 보강:
  - 우선 `item_data_table.md`, `encounter_data_tables.md`를 conventions 포맷으로 리라이트한다.

### 7. 허브 문서와 장비 문서가 v1 장비 슬롯 구성을 다르게 말한다

- 대상 문서:
  - `docs/proposal/main.md`
  - `docs/proposal/common/item.md`
- 문제:
  - `main.md`는 `무기 1 + 방어구 4 + 장신구 2`로 적고, `item.md`는 상의/하의/장갑 + 목걸이 + 반지 2개를 적어 실제 슬롯 구성이 다르다.
- 왜 중요한가:
  - `EquipSlot`과 장비 템플릿을 테이블로 굳히기 전에 슬롯 구성부터 닫혀 있어야 한다.
- 권장 보강:
  - v1 장비 슬롯 세트를 하나로 확정하고, 허브/세부 문서/테이블 문서를 동시에 맞춘다.

## Perspective Notes

### Planning

- `정찰 원정`, 플레이어 거래, 세부 UI 상태까지 데이터 테이블이 이미 꽤 많이 잠그고 있다.
- v1 컷라인이 더 줄어들면 관련 컬럼과 테이블 다수가 단순화될 수 있다.
- 따라서 현재 테이블은 "확정"이 아니라 "기본안"이라는 표시를 더 강하게 남겨야 한다.

### Server

- 드롭/보상/잠금 규칙은 런타임 DB 이전에 authoring 테이블 계약이 먼저 닫혀야 한다.
- 기믹과 보스 패턴도 `FailurePenaltyTag`, `RewardProfile`, `CheckpointResetRule`처럼 서버가 직접 읽는 rule key가 더 필요하다.

### Client

- 현재 문서는 수치와 구조는 강하지만, 렌더링 key와 상태 표기 reason code가 부족하다.
- HUD/툴팁/경고/차단 사유는 데이터 계약이 없으면 결국 UI 코드 상수로 분산될 가능성이 높다.

## OpenCode(GLM) Check

- `OpenCode(GLM)` 교차 검토를 절대경로 CLI(`D:\Users\pshpjr\AppData\Roaming\npm\opencode.cmd`)로 재실행했다.
- 독립 2차 검토에서도 큰 축은 동일했다:
  - `BossCombatProfileTable`에 `StaggerResistance`가 빠져 있어 `combat_formulas.md`의 무력화 공식 입력과 어긋난다.
  - `SkillLevelTable`, `SkillTripoTable`의 `BalanceVersion` 관리가 conventions 기준으로 약하다.
  - `item.md`와 `combat_stats.md`의 장비 슬롯 구성이 여전히 충돌한다.
- 추가로 OpenCode는 `encounter_data_tables.md`의 `DilTimeSec` 같은 명백한 컬럼명 오타 가능성과, `dungeon_entry.md`의 입장 차단 사유 enum 부재를 별도 리스크로 지적했다.
- 따라서 이번 문서는 Claude/Codex + 병렬 서브에이전트 + OpenCode(GLM) 2차 검토를 합친 결과다.

## Recommended Next Actions

1. `item_data_table.md`에 보상 프로필/전리품 선호 연결 구조를 추가한다.
2. `item_data_table.md`와 `combat_data_tables.md` 사이의 무기 authoring 소유권을 결정한다.
3. `encounter_data_tables.md`에 보스 공통 전투 프로필 테이블과 HUD/전조/기믹용 text/marker/severity key를 추가한다.
4. `main.md`와 `item.md`의 v1 장비 슬롯 구성을 먼저 통일한다.
5. `skill_data_table.md`에 슬롯 유효성 검증용 필드를 추가 검토한다.
6. `item_data_table.md`, `encounter_data_tables.md`를 `data-table-conventions.md` 포맷으로 재정리한다.
