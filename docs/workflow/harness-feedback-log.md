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


