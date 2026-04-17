# Exec Plan: office-hours 및 exec-plan 원칙 제거

> 생성: 2026-04-17
> 영역: `.claude/`, `.codex/`, `docs/workflow/`, `docs/AGENTS.md`, `AGENTS.md`

## 목표

Game Studio 기준에 맞춰 office-hours 체계와 exec-plan 강제 원칙을 저장소 기본 워크플로우에서 제거한다.

## 범위

- office-hours 에이전트/스킬 및 문서 삭제
- exec-plan-gate 스킬 및 exec-plan 강제 문구 제거
- AGENTS/workflow/settings 문서에서 exec-plan 의존 라우팅 제거

## 단계

- [x] 1. 제거 대상 파일/참조 수집
- [x] 2. office-hours 관련 파일 삭제 및 참조 정리
- [x] 3. exec-plan 강제 원칙 삭제 및 문서/설정 정리
- [x] 4. 문서 검증 실행 (`python tools/doc_check.py` 실행, 기존 저장소 이슈로 실패)
- [x] 5. plan 완료 및 인덱스 이동

## 완료 기준

- [x] office-hours 관련 엔트리 파일이 제거된다.
- [x] exec-plan을 작업 시작 필수 규칙으로 요구하지 않는다.
- [x] 관련 문서 링크가 깨지지 않도록 정리된다.


## 결과 요약

- office-hours 에이전트/스킬/워크플로 문서 제거
- exec-plan-gate 스킬 제거 및 주요 워크플로 문서에서 exec-plan 강제 규칙 제거
- settings/agent/skill 문구를 task notes 중심으로 치환
- `python tools/doc_check.py`는 기존 저장소 링크/형식 이슈로 실패
