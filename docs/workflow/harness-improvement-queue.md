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

## 2026-04-08 - command-script-usage-purpose-log
- status: open
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: medium
- token-meter: pending
- task: bash, PowerShell, Python 등 명령 실행 시 목적과 사용 스크립트를 구조화해 로깅하는 도구 또는 훅을 추가한다.
- message: 사용자가 에이전트가 `bash`, `python`, PowerShell 등으로 명령을 실행할 때 어떤 목적으로 어떤 스크립트나 커맨드를 사용했는지 기록하길 요청했다. 이 로그는 자주 쓰이는 스크립트와 반복 커맨드를 식별해 자동화, 스킬화, 문서 개선 피드백으로 연결하는 데 쓰인다. claim 시에는 기존 `_workspace/token-log.jsonl`, `_workspace/skill-usage-log.jsonl`, `tools/skill_usage_logger.py`, Claude Code hook 설정과 중복되지 않게 설계하고, command, purpose, script_path, cwd, result 정도의 최소 필드를 검토한다.
- source: 사용자 피드백 - 2026-04-08 명령/스크립트 사용 목적 로깅 요청

## 2026-04-08 - context-meter-encoding-tolerance
- status: open
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: medium
- token-meter: pending
- task: `context_meter.py`가 UTF-8 디코딩 오류를 만나도 원인 파일을 보고하고 계측을 계속할 수 있게 개선한다.
- message: 작업 중 `python tools/context_meter.py --git-base HEAD --files ...`가 `'utf-8' codec can't decode byte 0x91 in position 8` 오류로 실패해, 큐 항목의 token-meter를 `pending`으로 남겨야 했다. 다음 처리자는 파일별 예외 메시지에 경로를 포함하고, 필요하면 `errors=replace` 또는 명시적 fallback 정책을 적용해 계측이 완전히 중단되지 않도록 한다. 인코딩 문제를 숨기지 않도록 경고와 결과를 함께 출력하는 방식이 적합하다.
- source: 2026-04-08 startup-doc-discovery-delegation 큐 등록 중 `context_meter.py` 실패

## 2026-04-08 - startup-doc-discovery-delegation
- status: open
- suggested-agent: harness-improver
- area: workflow
- recommended-artifact: skill
- context-savings: high
- token-meter: pending
- task: 작업 시작 시 관련 문서 탐색을 경량 Codex 에이전트에 위임하는 라우팅 규칙을 설계하고 workflow 또는 skill에 반영한다.
- message: 사용자가 메인 에이전트가 작업 시작마다 AGENTS, exec-plan, 도메인 문서를 직접 넓게 탐색하지 않고, 경량 Codex 에이전트가 먼저 관련 문서를 훑은 뒤 실제로 읽어야 할 파일, 섹션, 라인과 이유만 반환하기를 요청했다. claim 시에는 `agent-workflow.md`, `exec-plan-gate`, `codex-delegate`의 책임 경계를 확인하고, 문서 탐색 결과가 숨은 추론이 아니라 짧은 파일-backed 요약으로 남도록 규칙화한다.
- source: 사용자 피드백 - 2026-04-08 작업 시작 문서 탐색 위임 요청

## 2026-04-08 - skill-agent-usage-counter
- status: done
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: medium
- token-meter: pending
- task: 스킬/에이전트 호출 횟수를 추적하는 스크립트 또는 훅을 추가한다.
- message: 현재 `_workspace/token-log.jsonl`은 세션 단위 토큰만 기록하며, 어떤 스킬·에이전트가 얼마나 자주 쓰이는지 알 수 없다. 사용 빈도 데이터가 없으면 미사용 에이전트 정리나 고사용 스킬 최적화 우선순위를 감에 의존해야 한다. Claude Code hooks(`PostToolUse`)를 활용해 `_workspace/skill-usage-log.jsonl`에 기록하거나, `record_agent_artifact.py` 확장으로 처리하는 방안 검토 필요.
- source: 사용자 피드백 — 2026-04-08 하네스 폴리싱 세션
- resolution: `tools/skill_usage_logger.py` 구현 + `.claude/settings.json` PostToolUse 훅 등록. 2026-04-08 처리 완료.

## 2026-04-08 - harness-improvement-discoverability
- status: done
- suggested-agent: harness-improver
- area: workflow
- recommended-artifact: doc
- context-savings: medium
- token-meter: pending
- task: 에이전트가 작업 중 harness-improvement 스킬을 언제 써야 하는지 명확한 트리거 규칙을 workflow 문서에 추가한다.
- message: 에이전트들이 harness-improvement 스킬의 존재를 알아도 호출 타이밍을 판단하기 어려워 개선 항목이 채팅에만 남거나 누락됨. `agent-workflow.md`에 트리거 조건을 명시해 모든 에이전트가 참조할 수 있게 함. 2026-04-08 폴리싱 세션에서 직접 처리 완료.
- source: 사용자 피드백 — 2026-04-08 하네스 폴리싱 세션

## 2026-04-05 - normalize-active-plan-goal-heading
- status: done
- suggested-agent: docs-reviewer
- area: workflow
- recommended-artifact: doc
- context-savings: low
- token-meter: pending
- task: Add or normalize the missing `## 목표` heading in active exec-plan files that still trigger `doc_check.py` format warnings.
- message: `20260404-office-hours-proposal-polish.md`는 completed/로 이동 후 삭제. `20260406-client-network-subsystem-proposal.md`에 동일 문제 발견 → `## 목표`, `## 완료 기준` 섹션 추가로 해결. `doc_check.py` 통과.
- source: `python tools/doc_check.py` during harness workflow updates

## 2026-04-05 - decide-whether-response-artifacts-should-also-capture-explic
- status: open
- suggested-agent: harness-improver
- area: workflow
- recommended-artifact: prompt
- context-savings: low
- token-meter: pending
- task: Decide whether response artifacts should also capture explicit command outputs for review-only runs.
- message: `_workspace/agent-notes/`는 존재함. 정책 미결: review-only 실행의 command output snippet을 agent-notes에도 저장할지, summary-only로 유지할지. 사용자 의사결정이 필요한 사항.
- source: recursive harness improvement rollout

## Claimed

없음.

## Done

- [2026-04-05 normalize-active-plan-goal-heading] 2026-04-08 처리 완료. `20260406-client-network-subsystem-proposal.md`에 `## 목표`, `## 완료 기준` 추가.
