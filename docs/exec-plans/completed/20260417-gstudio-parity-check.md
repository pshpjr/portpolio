# Exec Plan: Game Studio 원본 대비 잔존성 검증

> 생성: 2026-04-17
> 영역: `.claude/agents/`, `.claude/skills/`, `.codex/agents/`, `docs/workflow/`

## 목표

Game Studio 원본 자산이 삭제되지 않았음을 검증하고, codex wrapper 삭제 이유를 문서로 명확히 남긴다.

## 범위

- 원본(`Claude-Code-Game-Studios-main/.claude`) 대비 누락 agent/skill 체크
- codex wrapper 삭제 기준(대상 agent 삭제 시 동반 삭제) 문서화

## 단계

- [x] 1. 원본 대비 누락 항목 스크립트 검증
- [x] 2. 문서에 결과/정책 반영
- [x] 3. 문서 검증 실행 (`python tools/doc_check.py` 실행, 기존 저장소 이슈로 실패)
- [x] 4. plan 완료 및 인덱스 이동

## 완료 기준

- [x] Game Studio 원본 agent/skill 누락 0건이 확인된다.
- [x] wrapper 삭제 정책이 문서에 명시된다.


## 결과 요약

- 원본 대비 누락 항목 검사 결과: missing agents 0, missing skills 0
- non-studio 인벤토리에 검증 결과와 wrapper 삭제 기준 반영
- `python tools/doc_check.py`는 기존 저장소 링크/형식 이슈로 실패
