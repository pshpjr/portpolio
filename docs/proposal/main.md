# 포트폴리오 프로젝트 기획서: Dungeon of Spirits

> `docs/proposal/`의 허브 문서입니다.
> 여기서는 핵심 결정과 범위만 유지하고, 세부 규칙은 하위 문서로 분리합니다.

---

## 프로젝트 개요

- 목적: 이직/취업용 고성능 게임 서버 포트폴리오
- 장르: 1~4인 협동 인스턴스 던전 RPG
- 핵심 콘셉트: 패링 중심 전투, 무기 기반 올인 빌드, 협동 기믹 공략
- 기술 방향: C++20 커스텀 서버, Protobuf, MySQL/SQLite, 인던 단일 스레드 루프

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

- 무기 타입이 플레이 정체성과 스킬 구성을 결정한다.
- 패링, 무력화, 속성 상호작용이 전투의 핵심 축이다.
- 장착 무기 기준 스마트 드롭으로 올인 빌드를 지원한다.
- 스킬은 무기별 아이덴티티와 드롭 기반 성장 구조를 따른다.

관련 문서:
- [ingame/combat.md](./ingame/combat.md)
- [common/skill.md](./common/skill.md)
- [common/item.md](./common/item.md)

### 던전과 보상 방향

- 협동 기믹은 1~4인 파티 규모에 맞춰 스케일링된다.
- 기믹 실패는 보상 없는 페널티 몬스터 소환으로 처리한다.
- 보상은 제한형 메인 보상과 무제한 파밍 보상으로 이원화한다.
- 보스 공략은 패턴 이해, 무력화 타이밍, 역할 분담을 요구한다.

관련 문서:
- [ingame/dungeon_progress.md](./ingame/dungeon_progress.md)
- [ingame/boss_pattern.md](./ingame/boss_pattern.md)
- [ingame/monster_ai.md](./ingame/monster_ai.md)
- [outgame/dungeon_entry.md](./outgame/dungeon_entry.md)

### 아웃게임 방향

- 파티는 1~4인 기준이며, 솔로 입장도 동일한 파티 흐름으로 처리한다.
- 파티, 채팅, 입장 동의, 보상 모드 선택이 마을 플레이의 핵심 흐름이다.
- 경제는 수리, 재련, 거래 제한을 통해 아이템 가치를 유지한다.

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
| 1-B | 계정/마을 | 로그인, 캐릭터 선택, 마을 이동, 채팅, 파티 구성 |
| 2 | 인던 코어 | 던전 입장, 30fps 루프, 기본 공격, 사망/부활, 일반 몬스터 |
| 3 | 전투 심화 | 스킬, 패링, 무력화, 캔슬, 중간보스 |
| 4 | 보스 공략 | 보스 2페이즈, 협동 기믹, 패널티 소환, 드롭과 보상 |
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
- 무력화와 캔슬이 포함된 전투 시스템
- 최소 1종 이상의 보스 패턴과 보상 드롭
- 더미 클라이언트와 성능 검증 리포트

### 일정이 밀리면 뒤로 미룰 수 있는 항목

- 보스 2페이즈 확장 패턴
- 복잡한 협동 기믹 세트
- 기믹 실패 패널티 소환의 세부 확장
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

- [common/user.md](./common/user.md): 계정, 캐릭터, 공유 창고
- [common/item.md](./common/item.md): 아이템, 귀속, 랜덤 옵션
- [common/skill.md](./common/skill.md): 무기별 스킬, 성장, 스마트 드롭

### 인게임

- [ingame/combat.md](./ingame/combat.md): 전투 핵심 규칙
- [ingame/monster_ai.md](./ingame/monster_ai.md): 몬스터 역할과 어그로
- [ingame/boss_pattern.md](./ingame/boss_pattern.md): 보스 패턴과 공략 구조
- [ingame/dungeon_progress.md](./ingame/dungeon_progress.md): 구역 진행, 드롭, 기믹 실패

### 아웃게임

- [outgame/party.md](./outgame/party.md): 파티 생성, 초대, 상태 전이
- [outgame/chat.md](./outgame/chat.md): 채팅 채널과 메시지 흐름
- [outgame/economy.md](./outgame/economy.md): 상점, 수리, 재련, 거래
- [outgame/dungeon_entry.md](./outgame/dungeon_entry.md): 입장 동의, 보상 모드, 인원 스케일링

### 서버 기획

- [server/architecture.md](./server/architecture.md): 서버 구조와 스레드 모델
- [server/data_driven.md](./server/data_driven.md): 데이터 드리븐과 핫 리로드
- [server/performance_test.md](./server/performance_test.md): 부하 목표와 검증 방식
