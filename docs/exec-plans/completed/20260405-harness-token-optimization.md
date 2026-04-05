# Exec Plan: 하네스 토큰 최적화

## 목표

에이전트 작업에서 반복적으로 발생하는 패턴을 스크립트로 자동화하고, 에러 재시도 비용을 줄이며, codex/opencode 위임 기준을 명확히 해 세션당 토큰 사용량을 계측 가능하게 줄인다.

## 범위

**문서 구조 개선 (완료):**
- `docs/workflow/agent-workflow.md` — quick-ref로 축소 (1,666t → 404t)
- `docs/workflow/agent-workflow-detail.md` — 상세 절차 분리 (신규)
- `docs/workflow/harness-feedback-log.md` — 아카이브 규칙 + 오래된 항목 정리
- `docs/workflow/harness-feedback-log-archive.md` — 아카이브 (신규)

**스크립트 자동화 (구현 예정):**
- `tools/new_exec_plan.py` — exec-plan 보일러플레이트 생성 자동화
- `tools/check_all.py` — pre-commit 4개 검사 일괄 실행
- `tools/doc_token_audit.py` — 문서별 토큰 감사 (선택)

**위임 기준 명확화 (구현 예정):**
- `agent-workflow-detail.md`에 codex/opencode 위임 결정 트리 추가

## 완료 기준

- [x] agent-workflow.md quick-ref 분리 완료 (토큰 절감 1,262t/세션 계측)
- [x] feedback-log 아카이브 규칙 추가 + 정리
- [x] `tools/new_exec_plan.py` 구현 — exec-plan 생성 보일러플레이트 제거
- [x] `tools/check_all.py` 구현 — 4개 검사 명령 단일화
- [x] 위임 결정 기준 추가 (agent-workflow.md에 codex/opencode 위임 기준 추가)
- [x] 토큰 절감 측정 결과 이 plan에 기록
- [x] doc_check.py 통과 (기존 깨진 링크 수정 포함)

## 진행 상황

- [x] 세션 시작 문서 토큰 부하 측정 (agent-workflow.md 1,666t 확인)
- [x] agent-workflow.md → quick-ref + detail 분리
- [x] harness-feedback-log 아카이브 규칙 + 항목 정리
- [x] new_exec_plan.py 구현
- [x] check_all.py 구현
- [x] 위임 결정 기준 문서화
- [x] 최종 토큰 측정 및 기록

## 토큰 절감 측정 결과

| 문서 | 개선 전 | 개선 후 | 절감 |
|---|---|---|---|
| agent-workflow.md | 1,666t | 521t | -1,145t/세션 |
| harness-feedback-log.md | 1,071t | 843t | -228t/세션 (아카이브로 추가 감소 예정) |
| **세션 시작 7개 문서 합계** | **3,700t(추정)** | **2,914t** | **-786t/세션** |

신규 도구:
- `tools/new_exec_plan.py`: exec-plan 보일러플레이트 제거 → 형식 오류로 인한 재시도 제거
- `tools/check_all.py`: 4개 검사 → 1개 명령 → 빠진 검사로 인한 재커밋 제거
- `check_encoding.py`: `.venv`, `.idea` 제외 추가 → false positive 제거

## 설계 결정

- 문서 분리: quick-ref가 매 세션 로드되고 detail은 필요 시만 로드 → 약 1,262t/세션 절감
- 스크립트 우선순위: exec-plan 생성(매 세션 발생) > pre-commit 통합(PR 시 발생) > 토큰 감사(선택)
- 위임 기준: "300줄 이상 반복 구현" 또는 "검증 없이 재시도 2회 이상 예상"이면 codex-delegate 우선 검토
- 에러 재시도 제거: `check_all.py`로 빠진 검사로 인한 재커밋 제거 목표
