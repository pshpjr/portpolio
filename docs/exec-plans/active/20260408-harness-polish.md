# Exec Plan: 모노레포 에이전트 하네스 폴리싱

> 생성: 2026-04-08
> 영역: `.claude/`, `.codex/`, `docs/workflow/`, `docs/exec-plans/`

## 목표

3개 하위 영역을 가진 모노레포 구조에 맞게 에이전트 하네스를 정리하고, 쓰지 않는 스킬을 제거하거나 비활성화하며, 반복적으로 발견되는 개선점을 큐로 남겨 하네스가 점진적으로 개선되도록 만든다.

## 범위

- 루트 `.claude/agents/`, `.claude/skills/`, `.codex/skills/` 구조 점검
- 하위 영역(`server/`, `Lib/`, `client/`)별 라우팅과 책임 경계 점검
- `docs/workflow/`의 하네스 피드백/개선 큐 점검 및 보강
- 삭제 가능한 스킬과 보존해야 하는 스킬 분류

## 단계

- [x] 1. 현재 하네스 파일, 스킬, 에이전트, 워크플로 문서 목록화
- [x] 2. 모노레포 하위 영역별 라우팅 문제와 중복 규칙 식별
- [x] 3. 미사용/중복 스킬 제거 방식 검토
- [x] 4. 재귀 개선 큐가 `task`/`message` 중심으로 실행 가능하게 유지되는지 검증 및 보강
- [x] 5. 문서 검사 실행

## 완료 기준

- [x] 하위 영역별 하네스 진입 경로가 명확하다.
- [x] 제거 대상 스킬은 실제 참조 여부를 확인한 뒤 정리된다. (game-review 에이전트들은 plan-ceo-review에서 참조 → 보존)
- [x] 재귀 개선 큐가 후속 에이전트가 바로 실행할 수 있는 형태다.
- [x] `python tools/doc_check.py`가 통과한다.

## 결과 요약

- `.codex/agents/data-table-reviewer.md` 래퍼 추가 (.claude/.codex 동기화)
- `20260406-client-network-subsystem-proposal.md` 형식 오류 수정 (목표/완료기준 섹션 추가)
- `agent-workflow.md`에 harness-improvement 트리거 규칙 명시
- 개선 큐: normalize-active-plan-goal-heading → done, 신규 2건 등록 (skill-usage-counter, discoverability)
- 추가 사용자 피드백 반영: startup-doc-discovery-delegation 큐 등록 (작업 시작 문서 탐색을 경량 Codex 에이전트에 위임)
- 추가 사용자 피드백 반영: context-meter-encoding-tolerance 큐 등록 (`context_meter.py` UTF-8 디코딩 실패를 후속 개선으로 추적)
- 추가 사용자 피드백 반영: command-script-usage-purpose-log 큐 등록 (명령/스크립트 실행 목적과 사용 빈도 로깅)
- 추가 사용자 피드백 반영: `tools/find_doc_refs.py`를 표준 라이브러리 기반으로 정비하고 `doc-value-audit` 절차를 새 스크립트 호출 기준으로 갱신
