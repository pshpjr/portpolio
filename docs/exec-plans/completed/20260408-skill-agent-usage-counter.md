# Exec Plan: Skill / Agent 사용 카운터 구현

> 생성: 2026-04-08
> 영역: `tools/`, `.claude/settings.json`, `_workspace/`

## 목표

스킬·에이전트 호출 횟수를 자동으로 추적해 `_workspace/skill-usage-log.jsonl`에 기록한다.
Claude Code `PostToolUse` 훅으로 `Skill`·`Agent` 도구 호출을 감지하고, 미사용 스킬 정리와 고사용 스킬 최적화 우선순위를 데이터 기반으로 결정할 수 있게 한다.

## 범위

- `tools/skill_usage_logger.py` — PostToolUse 훅 스크립트
- `.claude/settings.json` — PostToolUse 훅 등록
- `_workspace/skill-usage-log.jsonl` — 로그 파일 (자동 생성)

## 단계

- [x] 1. `tools/skill_usage_logger.py` 작성
- [x] 2. `.claude/settings.json`에 `PostToolUse` 훅 추가
- [x] 3. 수동 테스트로 로그 기록 확인
- [x] 4. 개선 큐 항목 `skill-agent-usage-counter` → done 처리

## 완료 기준

- [x] `Skill` 도구 호출 시 skill 이름이 `skill-usage-log.jsonl`에 기록된다.
- [x] `Agent` 도구 호출 시 agent type과 description이 기록된다.
- [x] 훅 실패가 세션을 중단시키지 않는다 (exit 0 보장).

## 결과 요약

- `tools/skill_usage_logger.py` 생성 — PostToolUse stdin JSON을 파싱해 Skill/Agent 호출을 `_workspace/skill-usage-log.jsonl`에 JSONL로 기록
- `.claude/settings.json` `PostToolUse` 훅 추가 — matcher: `Skill|Agent`
- 수동 테스트 통과 (Skill, Agent 양쪽 모두)
