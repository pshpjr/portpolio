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
4. 처리 후 `python tools/hi_done.py <short-title>` 로 완료 처리한다 (큐에서 제거 + done 파일 이동).

## Open Tasks

## 2026-04-14 - document-gitbutler-cli-workflow-as-skill
- status: open
- suggested-agent: harness-improver
- area: skill
- recommended-artifact: skill
- context-savings: medium
- token-meter: pending
- task: GitButler CLI(`but`) 사용 흐름을 별도 문서 + skill로 정리한다. 스테이징/커밋/푸시, hunk lock 동작, 잠금된 hunk가 다른 브랜치로 자동 분리될 때의 처리 절차를 포함.
- message: clang-format 일괄 적용 작업에서 `but stage` → `but commit <branch> --only -m` → `but push <branch>` 루틴을 사용했는데, 파일 hunk가 기존 브랜치의 커밋(🔒)에 잠겨 다른 브랜치로 자동 스테이지되는 동작 때문에 동일한 포매팅 작업이 두 개 브랜치에 나뉘어 커밋되는 일이 반복됐다. 매 세션마다 CLI 헬프를 탐색하지 않도록 기본 절차, 잠금 처리 선택지(해당 브랜치에 같이 커밋 / unapply 후 재시도), 바이너리 경로(`/c/Program Files/GitButler/but.exe`), 단일 소스 위치(`.claude/skills/` 원본, `.codex/skills/` 래퍼) 규칙을 명시해야 한다.
- source: server/ clang-format sweep 세션 (chore/clang-format-sweep, boost-mysql-user-db-executor 분리 커밋 경험)

## 2026-04-08 - review-delegate-opencode-text-extraction-robustness
- status: open
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: high
- token-meter: pending
- task: `tools/review_delegate.sh`의 opencode JSON 후처리를 실제 다양한 출력으로 검증하고 edge case 보강
- message: opencode `--format json` 출력에서 마지막 `type:text` 이벤트의 `.part.text`를 추출하는 로직이 단일 테스트(짧은 프롬프트)만 거쳤다. 긴 리뷰, 멀티 턴, 에러 시 빈 출력 등 edge case 검증 필요. jq 없는 환경의 python fallback도 테스트 필요.
- source: tools/review_delegate.sh

## 2026-04-05 - decide-whether-response-artifacts-should-also-capture-explic
- status: dropped
- decision: summary-only — review-only 실행의 command output은 agent-notes에 저장하지 않는다.
- source: recursive harness improvement rollout

## Claimed

없음.

## Done

완료 항목은 [harness-improvement-done.md](./harness-improvement-done.md)에서 관리합니다.
