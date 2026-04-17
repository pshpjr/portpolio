# Exec Plan: plan-review 연계 legacy critic agents 제거

> 생성: 2026-04-17
> 영역: `.claude/agents/`, `.codex/agents/`, `docs/workflow/`

## 목표

이미 제거한 plan-ceo/plan-eng-review에 강결합되어 있던 legacy critic agents를 함께 제거한다.

## 범위

- `.claude/agents/`의 legacy critic 7종 삭제
- `.codex/agents/` 대응 wrapper 삭제
- 관련 정리 문서 참조 업데이트

## 단계

- [x] 1. 삭제 대상 agent/wrapper 확인
- [x] 2. 파일 삭제
- [x] 3. 문서 참조 정리
- [x] 4. 문서 검증 실행 (`python tools/doc_check.py` 실행, 기존 저장소 이슈로 실패)
- [x] 5. plan 완료 및 인덱스 이동

## 완료 기준

- [x] legacy critic agent 파일이 제거된다.
- [x] codex wrapper가 함께 제거된다.
- [x] 정리 문서가 현재 상태를 반영한다.


## 결과 요약

- plan-review 연계 legacy critic agents 7종 삭제
- `.codex/agents/` 대응 wrapper 동시 삭제
- non-studio/prune 문서와 관련 문구를 현재 상태로 갱신
- `python tools/doc_check.py`는 기존 저장소 링크/형식 이슈로 실패
