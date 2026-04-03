---
name: opencode-review
description: Delegate a review or inspection task to the local OpenCode AI (http://127.0.0.1:4096) for a second opinion. Use after Claude's own review, or when an independent perspective is needed on code, documentation, or proposal content.
---

# OpenCode 리뷰 위임 스킬

Claude의 리뷰와 별도로, 로컬 OpenCode AI(http://127.0.0.1:4096)에 추가 리뷰를 요청한다.
두 번째 관점을 얻어 blind spot을 줄이는 데 사용한다.

## 언제 사용할까

- 큰 PR 또는 아키텍처 변경 전 독립적 2nd-opinion이 필요할 때
- 문서 구조 개편 후 일관성 검증을 추가로 확인할 때
- 기획 결정의 논리적 결함을 다각도로 검토할 때
- Claude가 리뷰를 마쳤지만 확신이 낮을 때

## 기본 워크플로

1. **Claude가 먼저** 자체 리뷰 또는 관련 에이전트(reviewer, docs-reviewer 등)로 1차 분석을 마친다.
2. **리뷰 대상 파일 목록과 컨텍스트를 정리**한다.
3. **`opencode-reviewer` 서브에이전트에 위임**한다.
   - 리뷰 타입 명시: 코드 / 문서 / 기획
   - 리뷰 대상 파일 경로 목록
   - 핵심 판단 기준 (레이어 규칙, 역할 분리 원칙 등)
4. **OpenCode 결과를 Claude의 결과와 합산**해 최종 판단을 내린다.

## 위임 프롬프트 구성 예시

```
opencode-reviewer에게 전달할 내용:

리뷰 타입: 코드 리뷰
대상 파일: server/src/03_core/combat/CombatSystem.cpp
판단 기준:
  - 레이어 규칙: 03_core는 04_service, 05_network에 의존하면 안 됨
  - 단일 책임: 한 클래스에 너무 많은 역할이 있는지
  - 잠재 버그: null 체크, 범위 검사, 경쟁 조건
```

## 결과 통합 원칙

- OpenCode 결과와 Claude 결과가 일치하면 → 높은 신뢰로 결론
- OpenCode만 문제를 발견하면 → 해당 finding을 반드시 재검토
- Claude만 문제를 발견하면 → 유지 (OpenCode 미발견이 반증이 아님)
- 상충하면 → 사용자에게 두 관점을 모두 전달하고 판단 요청

## 주의사항

- OpenCode가 실행 중이지 않으면 이 스킬을 건너뛰고 이유를 보고한다.
- OpenCode 결과는 2nd-opinion이다. Claude의 판단을 대체하지 않는다.
- 리뷰 결과를 exec-plan에 기록할 필요는 없다 (volatile한 리뷰 의견은 plan에 남기지 않음).
