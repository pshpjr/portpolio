# Exec Plan: plan-ceo/plan-eng-review 스킬 제거

> 생성: 2026-04-17
> 영역: `.claude/skills/`, `.codex/skills/`, `.claude/agents/`, `docs/workflow/`

## 목표

Game Studio 외부 유산인 `plan-ceo-review`, `plan-eng-review` 스킬과 관련 참조를 제거한다.

## 범위

- `.claude/skills/plan-ceo-review`, `.claude/skills/plan-eng-review` 삭제
- `.codex/skills/plan-ceo-review`, `.codex/skills/plan-eng-review` 삭제
- 관련 agent/doc 참조 문구 정리

## 단계

- [x] 1. 삭제 대상 파일 및 참조 확인
- [x] 2. 스킬/메타데이터 파일 삭제
- [x] 3. 남은 참조 문서/agent 업데이트
- [x] 4. 문서 검증 실행 (`python tools/doc_check.py` 실행, 기존 저장소 이슈로 실패)
- [x] 5. plan 완료 및 인덱스 이동

## 완료 기준

- [x] 두 스킬이 `.claude`, `.codex`에서 제거된다.
- [x] 주요 참조 문구가 제거/대체된다.
- [x] 삭제 사실이 인벤토리 문서에 반영된다.


## 결과 요약

- `plan-ceo-review`, `plan-eng-review`를 .claude/.codex 양쪽에서 제거
- 관련 agent 입력 문구와 정리 문서 참조 업데이트
- non-studio 인벤토리/정리 후보 문서에 삭제 반영
- `python tools/doc_check.py`는 기존 저장소 링크/형식 이슈로 실패
