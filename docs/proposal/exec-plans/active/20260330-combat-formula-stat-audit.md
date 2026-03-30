# Combat Formula And Stat Audit

## 목표
`docs/proposal/` 안에서 전투 공식, 데미지 계산 규칙, 플레이어/장비 스탯 시스템이 어디까지 정의되어 있는지 확인하고, 후속 엑셀 데이터 설계에 필요한 누락 지점을 정리한다.

## 범위
- `docs/proposal/main.md`
- `docs/proposal/ingame/combat.md`
- `docs/proposal/common/skill.md`
- `docs/proposal/common/item.md`
- `docs/proposal/common/user.md`
- 필요 시 전투 연관 문서 (`ingame/boss_pattern.md`, `ingame/monster_ai.md`, `outgame/economy.md`)

## 완료 기준
- [x] 전투 공식이 명시된 문서 유무를 확인한다.
- [x] 플레이어/장비/전투 스탯 항목이 정의된 문서를 확인한다.
- [x] 후속 엑셀 설계에 필요한 최소 시트 후보와 누락 정보를 정리한다.

## 진행 상황
- [x] 관련 허브 문서와 active exec-plan 인덱스를 확인했다.
- [x] 전투 관련 핵심 문서를 읽고 정의 수준을 판별한다.
- [x] 조사 결과와 문서 공백을 사용자에게 정리한다.

## 조사 메모
- 현재 허브 문서에는 전투 축과 관련 문서 위치만 요약되어 있다.
- `ingame/combat.md`는 전투 규칙 허브일 가능성이 높고, 키워드 검색상 상세 공식은 구현 문서로 넘긴다는 표현이 보인다.
- 조사 결과, 전투의 구조와 루프는 정의되어 있지만 데미지 공식, 상태 이상 누적 공식, 자원 비용, 쿨다운 수치, 플레이어 기본 능력치 목록은 아직 고정되지 않았다.
- 현재 문서에 남아 있는 수치성 정보는 슬롯 수, 속성 종류, 일부 드롭/경제 정책, 보스 시간 제한 같은 운영 규칙 중심이다.
