# Exec Plan: Game Studio 자산 실구조 병합

> 생성: 2026-04-17
> 영역: `.claude/agents/`, `.claude/skills/`, `AGENTS.md`, `docs/AGENTS.md`, `docs/workflow/`

## 목표

`Claude-Code-Game-Studios-main/.claude/` 내부 agent/skill 데이터를 저장소의 canonical 위치(`.claude/agents`, `.claude/skills`)로 병합하고, 기존 문서 기반 중복 가이드는 정리한다.

## 범위

- Game Studio 에이전트 파일을 루트 `.claude/agents/`로 반영
- Game Studio 스킬 디렉터리를 루트 `.claude/skills/`로 반영
- 중복 파일(`performance-analyst.md`)은 단일 버전으로 정리
- 이전 중복 안내 문서(`game-studio-overlap-map.md`) 및 링크 정리

## 단계

- [x] 1. 병합 대상/중복 항목 식별
- [x] 2. agent/skill 데이터를 canonical 구조로 복사 및 중복 제거
- [x] 3. AGENTS/workflow 문서에서 병합 후 기준에 맞게 라우팅 정리
- [x] 4. 문서/구조 검증 실행 (`python tools/doc_check.py` 실행, 기존 저장소 이슈로 실패)
- [x] 5. plan 완료 처리 + 인덱스 이동

## 완료 기준

- [x] Game Studio agent/skill 데이터가 루트 canonical 구조에서 직접 사용 가능하다.
- [x] 중복되는 agent 파일은 단일 파일만 남는다. (performance-analyst 단일화)
- [x] 이전 임시 중복 맵 문서/링크가 정리된다.
- [x] 검증 결과가 기록된다.


## 결과 요약

- Game Studio 원본 `.claude/agents/*.md`를 루트 `.claude/agents/`로 병합
- Game Studio 원본 `.claude/skills/*`를 루트 `.claude/skills/`로 병합
- 중복 가이드 문서 대신 실제 자산 병합 상태를 기준으로 운영 문서 정리
- `python tools/doc_check.py`는 기존 저장소 링크/형식 경고로 실패(본 작업 외 이슈)
