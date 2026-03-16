# Exec Plan: Exec-Plan Gate And Skill

## 목표

- 서버 작업 전에 항상 exec-plan을 먼저 작성하도록 문서 규칙을 강화한다.
- Codex가 동일한 절차를 반복해서 따를 수 있도록 exec-plan 전용 skill을 추가한다.

## 범위

- `server/AGENTS.md`
- 필요 시 workflow 문서의 plan 관련 규칙
- 저장소 루트 `.codex/skills/exec-plan-gate/`
- 저장소 루트 `.claude/skills/exec-plan-gate/`

## 완료 기준

- `AGENTS.md`가 모든 작업 전에 exec-plan 작성과 사용자 검토를 요구한다.
- 새 skill이 plan 작성, active index 갱신, 사용자 검토 대기 절차를 명시한다.
- 문서와 skill이 현재 서버 작업 흐름과 충돌하지 않는다.

## 진행 상황

- [completed] exec-plan 초안 작성
- [completed] 문서 게이트 강화
- [completed] exec-plan skill 생성
- [completed] 변경 검토

## 설계 결정 기록

- plan 게이트는 권고가 아니라 금지 규칙으로 승격한다.
- skill은 구현을 시작하는 모든 서버 작업에서 트리거되도록 작성한다.
- skill은 코드 수정 전에 plan과 active index를 먼저 갱신하도록 지시한다.
- skill 저장 위치는 앱 디렉터리가 아니라 저장소 루트 `.codex/`, `.claude/`로 둔다.
