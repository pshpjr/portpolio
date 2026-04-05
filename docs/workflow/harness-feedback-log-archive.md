# Harness Feedback Log — 아카이브

현재 로그(`harness-feedback-log.md`)에서 이동된 오래된 항목을 보관합니다.

---

## 2026-04-04 - gstack-web-assumptions-must-be-trimmed
- kind: requested-change
- area: workflow
- summary: 사용자가 `gstack`를 하네스에 녹이되, 현재 레포 워크플로에 맞지 않는 웹 전용 요소는 버려야 한다고 명시했다.
- impact: 브라우저 QA와 배포 전제를 그대로 가져오면 문서 우선 게임 기획 루프가 오염되고 자동화 초점이 흐려질 수 있다.
- suggested-follow-up: `gstack` 적응 문서에서 유지/폐기 목록을 명시하고, office-hours 기반 기획/분석/교차검토 루프만 공식 경로로 남긴다.

## 2026-04-04 - separate-planning-skills-maximally
- kind: requested-change
- area: workflow
- summary: 사용자가 `office-hours`, `plan-ceo-review`, `plan-eng-review`, 성능 분석, 아키텍처 평가, 트레이드오프 해소를 최대한 분리된 스킬로 두자고 요청했다.
- impact: 평가 축이 한 스킬에 뭉치면 재실행 단위가 흐려지고 결과물 책임이 섞인다.
- suggested-follow-up: 상위 오케스트레이터 하나와 독립 평가 스킬 여러 개로 나누고, 필요할 때만 선택적으로 호출할 수 있게 유지한다.

## 2026-03-30 - branch-owned-commit-push-default
- kind: requested-change
- area: workflow
- summary: 사용자가 각 에이전트가 자기 브랜치에서 작업하고 주기적으로 `dev`를 merge 해 오며, 완료 시 자기 브랜치 푸시 후 `dev`에도 반영하도록 기본 동작을 바꿔 달라고 요청했다.
- impact: 브랜치 소유권과 `dev` 동기화 규칙이 명확하지 않으면 작업 완료 후 통합 방식이 세션마다 흔들릴 수 있었다.
- suggested-follow-up: 공용 workflow와 worker/scaffolder 프롬프트에 자기 브랜치 작업, 주기적 `dev` merge, 완료 후 `dev` 반영 규칙을 추가한다.
