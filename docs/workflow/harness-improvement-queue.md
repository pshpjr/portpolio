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
## 2026-04-08 - doc-ref-finder-script
- status: open
- suggested-agent: worker
- area: tool
- recommended-artifact: script
- context-savings: high
- token-meter: pending
- task: `tools/find_doc_refs.py <파일경로>` 스크립트를 작성한다 — 마크다운 파일명을 인자로 받아 저장소 전체에서 참조하는 파일 경로와 라인 번호를 출력
- message: doc-value-audit 실행 중 문서 참조 현황 파악에 문서당 Grep 1회 + 라인 확인용 Read가 반복됐다. 이번 감사(10개 문서)에서 약 15~20 툴 호출이 발생. 스크립트가 있으면 `Bash 1회`로 대체 가능. doc-value-audit 스킬의 "링크 참조 현황 확인(`Grep`으로 파일명 검색)" 단계를 이 스크립트 호출로 교체하고 스킬 설명도 업데이트해야 한다.
- source: doc-value-audit 실행 - 2026-04-08 서버 문서 검증

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

완료 항목은 [harness-improvement-done.md](./harness-improvement-done.md)에서 관리합니다.
