# Exec Plan: Gstack Office-Hours Harness Adaptation

## 목표

`gstack`의 office-hours 기반 사고 루프를 `portpolio` 하네스에 맞게 재구성해,
사용자 입력 한 번으로 기획 보강, CEO/ENG 관점 검토, 아키텍처 평가, 성능 검토,
트레이드오프 정리, GLM 2차 검토까지 이어지는 분리형 스킬 체계를 만든다.

## 범위

- `docs/exec-plans/active/INDEX.md`
- `docs/workflow/agent-workflow.md`
- `docs/workflow/gstack-office-hours-loop.md` 신규
- `docs/workflow/harness-feedback-log.md`
- `.claude/agents/office-hours-orchestrator.md` 신규
- `.claude/agents/server-proposal-critic.md` 신규
- `.claude/agents/client-proposal-critic.md` 신규
- `.claude/agents/architecture-evaluator.md` 신규
- `.claude/agents/performance-analyst.md` 신규
- `.claude/agents/tradeoff-moderator.md` 신규
- `.claude/skills/office-hours/SKILL.md` 신규
- `.claude/skills/plan-ceo-review/SKILL.md` 신규
- `.claude/skills/plan-eng-review/SKILL.md` 신규
- `.claude/skills/architecture-eval/SKILL.md` 신규
- `.claude/skills/performance-analysis/SKILL.md` 신규
- `.claude/skills/tradeoff-resolution/SKILL.md` 신규
- `.codex/agents/` 신규 참조 스텁 6개
- `.codex/skills/` 신규 참조 래퍼 및 `openai.yaml` 6세트

## 완료 기준

- [x] `gstack`에서 유지할 루프와 버릴 웹 전제가 문서화된다.
- [x] `office-hours`, `plan-ceo-review`, `plan-eng-review`, `architecture-eval`, `performance-analysis`, `tradeoff-resolution` 스킬이 분리 정의된다.
- [x] 필요한 신규 agent 정의가 `.claude/` 단일 소스로 추가된다.
- [x] `.codex/` 참조 스텁과 메타데이터가 함께 추가된다.
- [x] `python tools/doc_check.py`가 통과한다.

## 진행 상황

- [x] `gstack` 공개 저장소를 확인해 핵심 루프와 웹 전용 요소를 분리했다.
- [x] 분리 대상 스킬 목록을 사용자와 합의했다.
- [x] 루트 exec-plan과 workflow 문서를 추가한다.
- [x] 분리된 agent/skill 정의를 추가한다.
- [x] 문서 검증을 실행하고 결과를 plan에 반영한다.

## 설계 결정

- `gstack`를 그대로 벤더링하지 않는다. `office-hours`, 계획 검토, 평가, 트레이드오프 루프만 가져오고 웹 QA/배포/브라우저 연결 전제는 제거한다.
- `office-hours`는 상위 오케스트레이터 스킬로 두고, 나머지 평가는 독립 스킬로 분리한다. 그래야 특정 라운드에서 필요한 분석만 선택적으로 재실행할 수 있다.
- `plan-ceo-review`는 기존 `portfolio-scope-guardian` 등 기획 비평 에이전트를 조합해 상위 제품/범위 검토를 수행한다.
- `plan-eng-review`는 서버/클라 데이터 계약 검토에 집중시키고, 아키텍처/성능/트레이드오프는 별도 스킬로 분리한다.
- GLM 교차 검토는 새 스킬이 아니라 기존 `opencode-review` 경로를 재사용한다. 현재 루트 `opencode.json`의 기본 모델이 `zai-coding-plan/glm-5.1`이기 때문이다.

## 검증 기록

- `python tools/doc_check.py` → `OK: documentation checks passed.`
