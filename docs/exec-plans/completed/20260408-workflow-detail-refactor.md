# Exec Plan: agent-workflow-detail.md 재편

> 생성: 2026-04-08
> 영역: `docs/workflow/`

## 목표

`agent-workflow-detail.md`를 "에이전트 즉시 행동 규칙"만 담는 핫패스 문서로 슬림화하고,
운영 정책/보조 도구 규칙을 목적별 문서로 분리한다.

## 범위

- `docs/workflow/agent-workflow-detail.md` — 핫패스 규칙만 남기고 나머지 제거
- `docs/workflow/vcs-policy.md` — 브랜치/merge 정책 + 자동 커밋/푸시 정책 (신규)
- `docs/workflow/pr-checklist.md` — PR 제출 전 체크리스트 (신규)
- `docs/workflow/artifact-policy.md` — LLM 아티팩트 저장 규칙 + skill/agent 저장 위치 (신규)
- `docs/workflow/agent-workflow.md` — 피드백/개선 큐 참조 링크 확인

## 남길 것 (agent-workflow-detail.md)

- 빠른 참조 진입 안내
- 공유 브랜치에서 오래 작업하지 않는 원칙
- conflict/권한/원격 불일치 → blocker 보고
- 완료 상태 커밋 원칙 (선커밋)
- 무관한 dirty change 분리 원칙
- 막혔을 때 행동 원칙
- 분리 문서 참조 링크 섹션

## 단계

- [x] 1. vcs-policy.md 생성
- [x] 2. pr-checklist.md 생성
- [x] 3. artifact-policy.md 생성
- [x] 4. agent-workflow-detail.md 슬림화
- [x] 5. agent-workflow.md 참조 링크 확인/업데이트

## 완료 기준

- `agent-workflow-detail.md`가 에이전트 즉시 행동 규칙만 담는다 (100줄 이하 목표).
- 분리된 내용은 각 목적 문서에서 찾을 수 있다.
- 기존 링크가 깨지지 않는다.
