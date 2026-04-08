# AGENTS.md
> Core 레이어 작업 진입점.

## 이 디렉터리에서 먼저 확인할 것

1. `../../AGENTS.md`
2. `../../ARCHITECTURE.md`
3. `../../docs/conventions/cpp-style.md`

## Core 불변식

- Core는 `Types`, `Config`만 의존한다.
- Core는 네트워크, 세션, 런타임 개념을 알면 안 된다.
- 새 Core 로직에는 테스트 추가를 기본으로 한다.

## 하위 도메인

- `combat/`: 전투 판정과 결과 계산
- `inventory/`: 아이템 보관과 조작 규칙
- `stat/`: 수치 계산과 파생 스탯

## 변경 시 체크

- include 방향이 오른쪽 레이어를 가리키지 않는지 확인
- 순수 로직이 입출력, 시간, 세션 상태에 묶이지 않는지 확인
- 관련 단위 테스트를 추가 또는 갱신
