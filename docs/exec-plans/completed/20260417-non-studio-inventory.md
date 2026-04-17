# Exec Plan: 잔여 non-studio agent/skill 인벤토리

> 생성: 2026-04-17
> 영역: `.claude/agents/`, `.claude/skills/`, `docs/workflow/`

## 목표

현재 남아 있는 agent/skill 중 Game Studio 도메인 운영과 직접 관련 없는 항목을 분류해 사용자 의사결정을 돕는다.

## 범위

- `.claude/agents/` 전체에서 non-studio 후보 분류
- `.claude/skills/` 전체에서 non-studio 후보 분류
- 제거/유지 판단 기준을 함께 문서화

## 단계

- [x] 1. 현재 agent/skill 목록 스캔
- [x] 2. non-studio 후보와 근거 분류
- [x] 3. 결과 문서 작성
- [x] 4. 문서 검증 실행 (`python tools/doc_check.py` 실행, 기존 저장소 이슈로 실패)
- [x] 5. plan 완료 및 인덱스 이동

## 완료 기준

- [x] non-studio 후보가 파일명 기준으로 식별된다.
- [x] 왜 non-studio인지 분류 근거가 있다.
- [x] 즉시 제거 금지(운영 인프라) 항목이 구분된다.


## 결과 요약

- non-studio agent/skill 인벤토리 문서 추가
- Legacy/운영 인프라/문서 거버넌스/브리지 계층으로 분류
- 즉시 삭제 금지 항목과 1차 제거 후보를 구분
- `python tools/doc_check.py`는 기존 저장소 링크/형식 이슈로 실패
