# Harness Feedback Log

하네스 사용 중 나온 피드백과 작업 이슈를 짧게 남기는 공용 로그입니다.

## 목적

- 반복되는 사용자 피드백을 모은다.
- 작업 중 자주 막히는 지점을 모은다.
- 나중에 개선 에이전트가 문서, 규칙, 프롬프트, 프로세스 개선 후보를 추출할 수 있게 한다.

## 기록 원칙

- 길게 쓰지 않는다.
- 실제로 개선으로 이어질 만한 사건만 남긴다.
- 같은 문제의 장황한 재서술보다, 패턴과 원인을 짧게 적는다.
- 민감한 비밀값, 토큰, 개인정보는 적지 않는다.

## 언제 기록할까

- 사용자가 답변 방식이나 작업 흐름을 수정해 달라고 요청했을 때
- 같은 종류의 혼선이나 되돌림이 반복될 때
- 구현/검증 중 도구, 문서, 규칙의 빈틈 때문에 작업이 느려졌을 때
- 빌드 / 테스트 등의 작업이 한번에 성공하지 않았을 때
- 앞으로의 에이전트 동작 규칙에 반영할 가치가 있다고 판단될 때

## 기록 형식

각 항목은 아래 템플릿을 따른다.

```markdown
## YYYY-MM-DD - short-title
- kind: user-feedback | requested-change | implementation-issue | process-gap
- area: docs | workflow | agent-prompt | tool | build | test | other
- summary: 한두 문장으로 사건 요약
- impact: 무엇이 느려졌거나 어긋났는지
- suggested-follow-up: 문서 수정, 프롬프트 수정, 스크립트 추가 등 다음 조치
```

## Entries

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


