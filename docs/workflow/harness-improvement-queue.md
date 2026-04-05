# Harness Improvement Queue

하네스가 작업 중 발견한 재사용 가능한 개선 항목을 남기는 작업 큐입니다.

이 문서는 채팅 로그 보관용이 아닙니다.
다음 에이전트가 바로 집어가서 처리할 수 있는 독립 작업만 남깁니다.

## 언제 남길까

- 작업 중 문서, 프롬프트, 워크플로, 도구 규칙의 빈틈 때문에 우회가 필요했을 때
- 작업은 끝났지만, 같은 문제를 다음 세션에서도 반복할 가능성이 높을 때
- 현재 메인 작업과 분리 가능한 후속 정리가 남았을 때

## 무엇을 남기지 말까

- 지금 세션 안에서 바로 고칠 수 있는 사소한 메모
- 사용자의 의사결정이 먼저 필요한 미확정 사항
- 장문의 사건 서술 또는 대화 복사본

## 항목 형식

각 항목은 아래 필드를 포함한다.

```markdown
## YYYY-MM-DD - short-title
- status: open | claimed | done | dropped
- suggested-agent: harness-improver | docs-reviewer | reviewer | worker | other
- area: workflow | docs | agent-prompt | skill | tool | build | test | other
- recommended-artifact: script | skill | prompt | doc | other
- context-savings: low | medium | high | unknown
- token-meter: `<estimated tokens / changed lines>` | pending
- task: 다음 에이전트가 수행할 작업 한 줄
- message: 왜 이 작업이 필요한지, 어디서 드러났는지, 다음 에이전트가 알아야 할 핵심 맥락
- source: 발견한 작업 또는 파일 경로
```

핵심 규칙:

- `task`는 명령형으로 짧게 쓴다.
- `message`는 재현 맥락과 의도를 남긴다.
- 반복 설명이나 반복 생성이 원인이라면 `recommended-artifact`에 `script` 또는 `skill`을 명시한다.
- 컨텍스트 / 토큰 절감 기대치가 보이면 `context-savings`에 강도를 남긴다.
- substantial item이면 `token-meter`에 계측 결과를 남기고, 아직 안 쟀으면 `pending`으로 둔다.
- 하나의 항목에는 하나의 독립 작업만 남긴다.
- 처리자가 정해지면 `status: claimed`와 함께 본문에 owner 메모를 덧붙여도 된다.

## 운영 방식

1. 메인 작업 에이전트는 작업 중 또는 종료 직전에 큐 필요 여부를 판단한다.
2. 독립 후속 작업이면 이 파일의 `Open Tasks`에 새 항목을 추가한다.
3. `harness-improver` 또는 더 적합한 리뷰/구현 에이전트가 항목을 claim 한다.
4. 처리 후 결과를 반영하고 `status: done`으로 바꾼다.

## Open Tasks

## 2026-04-05 - normalize-active-plan-goal-heading
- status: open
- suggested-agent: docs-reviewer
- area: workflow
- recommended-artifact: doc
- context-savings: low
- token-meter: pending
- task: Add or normalize the missing `## 목표` heading in active exec-plan files that still trigger `doc_check.py` format warnings.
- message: `python tools/doc_check.py` passes, but `docs/exec-plans/active/20260404-office-hours-proposal-polish.md` still emits a format warning because it lacks the expected `## 목표` section. This is independent cleanup and should not block unrelated harness work.
- source: `python tools/doc_check.py` during harness workflow updates

## 2026-04-05 - decide-whether-response-artifacts-should-also-capture-explic
- status: open
- suggested-agent: harness-improver
- area: workflow
- recommended-artifact: prompt
- context-savings: low
- token-meter: pending meter
- task: Decide whether response artifacts should also capture explicit command outputs for review-only runs.
- message: The workflow now writes structured response notes, but the retention scope for command output snippets versus summary-only notes remains an open policy choice.
- source: recursive harness improvement rollout

## Claimed

없음.

## Done

없음.
