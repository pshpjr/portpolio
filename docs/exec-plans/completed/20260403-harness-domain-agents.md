# Exec Plan: 하네스 도메인 특화 — Codex 위임 스킬 + 전문 에이전트

## 목표

범용 worker/scaffolder로는 매 작업마다 레이어 규칙, 코드 생성 패턴을 처음부터 재로딩해야 하는
토큰 낭비와 실수 위험을 해소한다. 반복·결정적 작업은 Codex 서브에이전트에 위임하고,
C++ 서버 구현과 코드 생성에 특화된 도메인 에이전트를 추가한다.

## 범위

- `.claude/skills/codex-delegate/SKILL.md` — 신규
- `.claude/agents/server-implementer.md` — 신규
- `.claude/agents/codegen-specialist.md` — 신규
- `.claude/agents/design-author.md` — 신규
- `docs/exec-plans/active/INDEX.md` — 갱신

## 완료 기준

- [x] `codex-delegate` 스킬: 위임 판단 기준 + Codex 호출 프로토콜 + 결과 통합 방법 포함
- [x] `server-implementer` 에이전트: 레이어 규칙 내재화, exec-plan 게이트 준수, 빌드 검증 포함
- [x] `codegen-specialist` 에이전트: 스탯/프로토콜 생성 패턴, Python tools/ 활용법 포함
- [x] `design-author` 에이전트: proposal/ 전담, scope guard, 워크트리 운용 규칙 포함
- [x] 기존 에이전트(worker, scaffolder)와 역할 충돌 없음

## 진행 상황

- [x] exec-plan 작성
- [x] `codex-delegate` 스킬 작성
- [x] `server-implementer` 에이전트 작성
- [x] `codegen-specialist` 에이전트 작성
- [x] `design-author` 에이전트 작성
- [x] 역할 충돌 검토

## 설계 결정

- `worker`는 범용(문서 작업, 소규모 수정)으로 유지. `server-implementer`는 C++ 레이어 작업 전용.
- `codegen-specialist`는 Python tools/ 실행 포함 — Codex에 위임하기 적합한 대표 케이스.
- Codex 위임 기준: "결과가 결정적이고(같은 입력 → 같은 출력), 컨텍스트 독립적인 작업"은 Codex에 위임.
  Claude는 설계 결정, 조율, 사용자 소통에 집중.
