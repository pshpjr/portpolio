# Agent Workflow
> 에이전트 작업 절차의 단일 소스.
> 환경 세팅과 상세 검증 명령은 별도 참조 문서로 분리한다.

## 세션 시작 체크리스트

1. `AGENTS.md`를 읽는다.
2. `docs/exec-plans/active/INDEX.md`를 읽고 관련 plan만 연다.
3. 수정 경로에 맞는 `docs/context-map.md`와 로컬 `AGENTS.md`를 읽는다.
4. 관련 active plan이 없으면 새 exec-plan을 작성하고 active index를 갱신한다.
5. exec-plan 초안을 사용자에게 검토받고 승인 또는 수정 방향을 확인한다.
6. 그 뒤에만 레이어 규칙과 관련 컨벤션을 확인한 뒤 구현한다.

---

## exec-plan 규칙

- 위치: `docs/exec-plans/active/`
- 파일명: `YYYYMMDD-작업명.md`
- 완료 후 `completed/`로 이동
- 서버의 모든 작업은 exec-plan 작성 또는 기존 plan 확인으로 시작
- exec-plan 초안을 사용자에게 보여 주기 전에는 구현 파일 수정 금지

필수 섹션:

- 목표
- 범위
- 완료 기준
- 진행 상황
- 설계 결정 기록

---

## PR 전 검증 규칙

- 관련 빌드가 통과해야 한다.
- 관련 테스트가 통과해야 한다.
- `../tools/check_layers.py`가 통과해야 한다.
- 텍스트 파일을 바꿨다면 `../tools/check_encoding.py`를 통과해야 한다.
- 변경 범위에 맞는 문서와 exec-plan 상태를 갱신해야 한다.

상세 명령은 [verification.md](./verification.md)를 따르고, Windows 환경 장애 대응은 [build-troubleshooting.md](./build-troubleshooting.md)를 따른다.

---

## 막혔을 때 원칙

1. 추측으로 밀어붙이지 않는다.
2. 빠진 것이 문서인지, 컨벤션인지, 자동 검사인지 분류한다.
3. 부족한 지점을 문서 또는 도구로 보강한 뒤 다시 진행한다.

---

## 금지 행동

- 레이어 위반 코드 커밋
- Core 신규 로직에 테스트 없이 진행
- 관련 검증 없이 PR 준비
- exec-plan 없이 어떤 서버 작업이든 시작
- 사용자 검토 전 구현 파일 수정
- 설계 결정을 코드 뒤에만 남기고 문서에 남기지 않음
