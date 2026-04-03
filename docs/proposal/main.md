# 포트폴리오 프로젝트 기획서: Dungeon of Spirits

> `docs/proposal/`의 허브 문서입니다.
> 여기서는 핵심 결정과 범위만 유지하고, 세부 규칙은 하위 문서로 분리합니다.

---

## 프로젝트 개요

- 목적: 이직/취업용 고성능 게임 서버 포트폴리오
- 장르: 1~4인 협동 인스턴스 던전 RPG
- 핵심 콘셉트: 패링 중심 전투, 무기 전환 기반 빌드 운용, 협동 기믹 공략
- 기술 방향: C++20 커스텀 서버, Protobuf, MySQL/SQLite, 인던 단일 스레드 루프

플레이어는 오염된 성소를 정화하는 원정대가 되어, 정령 폭주와 주술 의식을 끊어내며 보스를 공략한다.
전투는 `패링 -> 빈틈 확보 -> 무력화 집중`의 반복으로 이해되어야 하며, 화면과 UI도 이 코어 루프를 바로 읽게 만드는 방향으로 설계한다.

관련 문서:
- [pending-decisions.md](./pending-decisions.md)

---

## v1 Showcase Slice

첫 완성본은 "한 번 설명하면 바로 이해되는 협동 액션 서버 포트폴리오"를 목표로 한다.

### v1에서 반드시 보여줄 것

- 마을 1곳, 고정 던전 `잿빛 성소`, 보스 `혼식 주술사`로 이어지는 완결된 플레이 루프
- 검방, 대검, 지팡이 3개 무기 타입과 각 무기별 전투 리듬
- `패링/카운터`, `무력화/다운`, `입장 시 잠기는 전리품 선호 무기`가 보이는 전투/보상 구조
- 마을에서 가능한 무기 변경, 무기별 스킬 프리셋, 던전 내부 잠금 규칙
- 개인 드롭, 장비 수리, 제한된 재련, 정화/정찰 원정으로 나뉘는 최소 경제 루프
- 단일 스레드 인던 루프와 성능 리포트로 설명 가능한 서버 기술 데모

### v1 데모 계약

- 보스는 `읽기 쉬운 1페이즈 코어 패턴 + 무력화 딜 타임`을 우선 보여준다.
- 전투 문서에는 카메라 거리, 화면 구도, HUD, 보스 전조 가독성 같은 핵심 플레이 표현 요소를 포함한다.
- 무기 변경은 가능하지만 `마을 또는 입장 준비 구간`에서만 허용하고, 던전 입장 후에는 현재 무기와 스킬 구성이 잠긴다.
- 경제는 `개인 드롭 + 수리 + 제한적 재련`을 기본으로 설명하고, 복잡한 거래 운영은 보조 요소로 다룬다.

### v1에서 고정할 범위

- 장비 구조는 `무기 1 + 방어구 4 + 장신구 2`의 단순 슬롯 체계로 시작한다.
- 전투 스탯은 `레벨 기본값 + 무기 기본값 + 장비 고정 스탯 + 랜덤 옵션 + 임시 버프` 조합으로 설명 가능해야 한다.
- 스마트 드롭은 특정 무기를 완전히 고정하지 않고, 던전 입장 시 선택한 전리품 선호 무기에 soft bias를 준다.
- 보상 모드, 경제, 속성, 위협도는 v1 코어를 보조하는 수준까지만 문서화한다.

### v1 이후로 미루는 것

- 보스 2페이즈 확장 패턴과 복잡한 협동 기믹 세트
- 속성 상성 심화, 상태 이상 세부 규칙, 위협도 감쇠 예외
- 공개 파티 자동화, 완성형 핫 리로드 운영 도구, 성능 리포트 자동 정리
- 절차적 던전, 마을/인던 물리 분리, Redis 기반 글로벌 세션

---

## 핵심 결정

### 아키텍처와 서버 방향

- 시작 구조는 모듈형 모놀리스다.
- 각 던전 인스턴스는 독립적인 단일 스레드 루프에서 처리한다.
- 계정 단위 공유 창고를 제공해 멀티 캐릭터 운용을 지원한다.
- 현재는 단일 프로세스를 전제로 하되, 이후 마을 서버와 인던 서버 분리를 고려한다.

관련 문서:
- [server/architecture.md](./server/architecture.md)
- [server/data_driven.md](./server/data_driven.md)
- [server/performance_test.md](./server/performance_test.md)

### 전투와 성장 방향

- 무기 타입은 플레이 정체성을 만들지만, 캐릭터는 마을에서 무기를 변경할 수 있다.
- 패링, 무력화, 화면 가독성이 전투의 핵심 축이다.
- 전투 문서는 조작, 카메라, 화면 구성, HUD까지 포함해 `핵심 플레이가 어떻게 읽히는지`를 설명한다.
- 스킬은 무기별 아이덴티티와 드롭 기반 성장 구조를 따르며, 무기별 프리셋으로 관리한다.

관련 문서:
- [ingame/combat.md](./ingame/combat.md)
- [common/skill.md](./common/skill.md)
- [common/item.md](./common/item.md)

### 던전과 보상 방향

- 협동 기믹은 1~4인 파티 규모에 맞춰 스케일링된다.
- 기믹 실패는 긴 지연보다 `짧은 복구 구간 + 명확한 페널티`로 처리한다.
- 보상은 `정화 원정(메인 보상)`과 `정찰 원정(반복 연습/파밍)`으로 나눈다.
- 보스 공략은 패턴 이해, 무력화 타이밍, 역할 분담을 요구한다.

관련 문서:
- [ingame/dungeon_progress.md](./ingame/dungeon_progress.md)
- [ingame/boss_pattern.md](./ingame/boss_pattern.md)
- [ingame/monster_ai.md](./ingame/monster_ai.md)
- [outgame/dungeon_entry.md](./outgame/dungeon_entry.md)

### 아웃게임 방향

- 파티는 1~4인 기준이며, 솔로 입장도 동일한 파티 흐름으로 처리한다.
- 파티, 채팅, 입장 동의, 빠른 핑, 재도전 준비가 마을 플레이의 핵심 흐름이다.
- 경제는 수리와 제한적 재련을 중심으로 아이템 가치를 유지한다.

관련 문서:
- [outgame/party.md](./outgame/party.md)
- [outgame/chat.md](./outgame/chat.md)
- [outgame/economy.md](./outgame/economy.md)
- [common/user.md](./common/user.md)

---

## 마일스톤 요약

| Phase | 목표 | 핵심 체크포인트 |
|---|---|---|
| 1-A | 서버 코어 | 클라이언트 접속, 패킷 송수신, 워커/DB/정적 테이블 로드 |
| 1-B | 계정/마을 | 로그인, 캐릭터 선택, 마을 이동, 채팅, 파티 구성, 무기 전환 |
| 2 | 인던 코어 | 던전 입장, 30fps 루프, 기본 공격, 사망/부활, 일반 몬스터 |
| 3 | 전투 심화 | 스킬, 패링, 무력화, 화면/HUD 가독성, 중간보스 |
| 4 | 보스 공략 | 혼식 주술사, 협동 기믹, 드롭과 보상 |
| 5 | 런타임/성능 | 핫 리로드, 더미 클라이언트, 메트릭 수집, 성능 리포트 |

기간 정책:
- 월 56시간 정도를 기준으로 Phase당 1개월을 기본 단위로 본다.
- XL 항목이 몰린 구간이나 보스/검증 구간은 2개월까지 늘어날 수 있다.

---

## 우선순위

### 반드시 있어야 하는 항목

- TCP/Protobuf 기반 통신 프레임워크
- 멀티스레드 구조와 인던 단일 스레드 루프
- 비동기 DB 레이어와 메모리 캐싱
- 무력화와 인터럽트가 포함된 전투 시스템
- 최소 1종 이상의 보스 패턴과 보상 드롭
- 더미 클라이언트와 성능 검증 리포트
- 전투 가독성을 설명할 수 있는 카메라/HUD 정의

### 일정이 밀리면 뒤로 미룰 수 있는 항목

- 보스 2페이즈 확장 패턴
- 복잡한 협동 기믹 세트
- 기믹 실패 페널티의 세부 확장
- 계정 공유 창고의 완전 구현
- 런타임 핫 리로드의 완성형 운영 도구
- 보상 이원화의 세부 확장
- 공개 파티 찾기 자동화
- 성능 리포트 자동 정리

---

## 확장 비전

- 마을 서버와 인던 서버의 물리 분리
- 절차적 던전 생성 도입
- Redis 기반 글로벌 세션 및 랭킹 공유

이 항목들은 포트폴리오 완성 후 확장성 증명 단계에서 다룬다.

---

## 하위 문서 지도

### 공통

- [common/user.md](./common/user.md): 계정, 캐릭터, 공유 창고, 무기 프리셋
- [common/login.md](./common/login.md): 로그인, 세션, 계정 생성
- [common/character_creation.md](./common/character_creation.md): 캐릭터 생성, 시작 무기, 초기 지급
- [common/progression.md](./common/progression.md): 경험치, 레벨업, 스킬 포인트, 프리셋 확장
- [common/item.md](./common/item.md): 아이템, 귀속, 랜덤 옵션, 전리품 선호 무기
- [common/item_data_table.md](./common/item_data_table.md): 아이템/드롭 authoring 테이블 구조
- [common/skill.md](./common/skill.md): 무기별 스킬, 성장, 무기 전환 규칙

### 인게임

- [ingame/combat.md](./ingame/combat.md): 전투 핵심 규칙, 카메라, HUD, 화면 구성
- [ingame/combat_ui_wireframe.md](./ingame/combat_ui_wireframe.md): 전투 HUD 와이어프레임과 정보 계층
- [ingame/combat_stats.md](./ingame/combat_stats.md): 전투 스탯 초안과 분류
- [ingame/combat_formulas.md](./ingame/combat_formulas.md): 전투 공식 기반 구조
- [ingame/combat_data_tables.md](./ingame/combat_data_tables.md): 전투 데이터 테이블과 레벨별 스탯 인덱스
- [ingame/encounter_data_tables.md](./ingame/encounter_data_tables.md): 보스 패턴, 전조, 체크포인트, 기믹 authoring 테이블
- [ingame/monster_ai.md](./ingame/monster_ai.md): 몬스터 역할과 어그로
- [ingame/boss_pattern.md](./ingame/boss_pattern.md): 혼식 주술사 패턴과 공략 구조
- [ingame/dungeon_progress.md](./ingame/dungeon_progress.md): 구역 진행, 드롭, 기믹 실패, 복구 리듬
- [ingame/death_revival.md](./ingame/death_revival.md): 사망, 부활, 전멸 처리
- [ingame/skill_data_table.md](./ingame/skill_data_table.md): 스킬 테이블 구조와 v1 스킬 목록

### 아웃게임

- [outgame/party.md](./outgame/party.md): 파티 생성, 초대, 재접속, 중도 포기
- [outgame/chat.md](./outgame/chat.md): 채팅 채널, 빠른 핑, 시스템 알림
- [outgame/economy.md](./outgame/economy.md): 상점, 수리, 재련, 거래
- [outgame/dungeon_entry.md](./outgame/dungeon_entry.md): 입장 동의, 원정 모드, 전리품 선호, 인원 스케일링

### 서버 기획

- [server/architecture.md](./server/architecture.md): 서버 구조와 스레드 모델
- [server/data_driven.md](./server/data_driven.md): 데이터 드리븐과 핫 리로드
- [server/performance_test.md](./server/performance_test.md): 부하 목표와 검증 방식
