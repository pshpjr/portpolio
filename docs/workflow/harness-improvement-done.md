# Harness Improvement — Done

처리 완료된 항목 아카이브입니다.
큐(`harness-improvement-queue.md`)에서 `status: done`이 된 항목을 이곳으로 옮깁니다.

---

## 2026-04-08 - context-meter-encoding-tolerance
- status: done
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: medium
- task: `context_meter.py`가 UTF-8 디코딩 오류를 만나도 원인 파일을 보고하고 계측을 계속할 수 있게 개선한다.
- message: 작업 중 `python tools/context_meter.py --git-base HEAD --files ...`가 `'utf-8' codec can't decode byte 0x91 in position 8` 오류로 실패해, 큐 항목의 token-meter를 `pending`으로 남겨야 했다. 다음 처리자는 파일별 예외 메시지에 경로를 포함하고, 필요하면 `errors=replace` 또는 명시적 fallback 정책을 적용해 계측이 완전히 중단되지 않도록 한다. 인코딩 문제를 숨기지 않도록 경고와 결과를 함께 출력하는 방식이 적합하다.
- source: 2026-04-08 startup-doc-discovery-delegation 큐 등록 중 `context_meter.py` 실패
- resolution: `read_file()`은 UTF-8 실패 시 경고 출력 후 latin-1 fallback. untracked 파일 루프는 인코딩 오류 시 경고 출력 후 해당 파일 line count 0으로 처리, 계측 계속. 2026-04-08 처리 완료.

---

## 2026-04-08 - startup-doc-discovery-delegation
- status: done
- suggested-agent: harness-improver
- area: workflow
- recommended-artifact: skill
- context-savings: high
- task: 작업 시작 시 관련 문서 탐색을 경량 Codex 에이전트에 위임하는 라우팅 규칙을 설계하고 workflow 또는 skill에 반영한다.
- message: 사용자가 메인 에이전트가 작업 시작마다 AGENTS, exec-plan, 도메인 문서를 직접 넓게 탐색하지 않고, 경량 Codex 에이전트가 먼저 관련 문서를 훑은 뒤 실제로 읽어야 할 파일, 섹션, 라인과 이유만 반환하기를 요청했다. claim 시에는 `agent-workflow.md`, `exec-plan-gate`, `codex-delegate`의 책임 경계를 확인하고, 문서 탐색 결과가 숨은 추론이 아니라 짧은 파일-backed 요약으로 남도록 규칙화한다.
- source: 사용자 피드백 - 2026-04-08 작업 시작 문서 탐색 위임 요청
- resolution: 2026-04-08 처리 완료.

---

## 2026-04-08 - skill-agent-usage-counter
- status: done
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: medium
- task: 스킬/에이전트 호출 횟수를 추적하는 스크립트 또는 훅을 추가한다.
- message: 현재 `_workspace/token-log.jsonl`은 세션 단위 토큰만 기록하며, 어떤 스킬·에이전트가 얼마나 자주 쓰이는지 알 수 없다. 사용 빈도 데이터가 없으면 미사용 에이전트 정리나 고사용 스킬 최적화 우선순위를 감에 의존해야 한다.
- source: 사용자 피드백 — 2026-04-08 하네스 폴리싱 세션
- resolution: `tools/skill_usage_logger.py` 구현 + `.claude/settings.json` PostToolUse 훅 등록. 2026-04-08 처리 완료.

---

## 2026-04-08 - harness-improvement-discoverability
- status: done
- suggested-agent: harness-improver
- area: workflow
- recommended-artifact: doc
- context-savings: medium
- task: 에이전트가 작업 중 harness-improvement 스킬을 언제 써야 하는지 명확한 트리거 규칙을 workflow 문서에 추가한다.
- message: 에이전트들이 harness-improvement 스킬의 존재를 알아도 호출 타이밍을 판단하기 어려워 개선 항목이 채팅에만 남거나 누락됨. `agent-workflow.md`에 트리거 조건을 명시해 모든 에이전트가 참조할 수 있게 함.
- source: 사용자 피드백 — 2026-04-08 하네스 폴리싱 세션
- resolution: 2026-04-08 폴리싱 세션에서 직접 처리 완료.

---

## 2026-04-05 - normalize-active-plan-goal-heading
- status: done
- suggested-agent: docs-reviewer
- area: workflow
- recommended-artifact: doc
- context-savings: low
- task: Add or normalize the missing `## 목표` heading in active exec-plan files that still trigger `doc_check.py` format warnings.
- message: `20260404-office-hours-proposal-polish.md`는 completed/로 이동 후 삭제. `20260406-client-network-subsystem-proposal.md`에 동일 문제 발견 → `## 목표`, `## 완료 기준` 섹션 추가로 해결.
- source: `python tools/doc_check.py` during harness workflow updates
- resolution: `20260406-client-network-subsystem-proposal.md`에 `## 목표`, `## 완료 기준` 추가. 2026-04-08 처리 완료.

---

## 2026-04-08 - add-codex-review-skill
- status: done
- suggested-agent: harness-improver
- area: skill
- recommended-artifact: skill
- context-savings: medium
- token-meter: pending
- task: `opencode-review`와 대칭되는 `codex-review` 스킬을 `.claude/skills/codex-review/SKILL.md`에 추가한다.
- message: 현재 외부 리뷰 수단이 `opencode-review`만 존재한다. 사용자가 Codex도 외부 리뷰 스킬로 추가하길 요청했다. `codex-review` 스킬은 `opencode-review`와 같은 역할이나 Codex CLI(`codex exec --sandbox read-only`)를 사용하고, 결과 통합 원칙도 동일하게 적용한다. `.codex/skills/`에는 참조 래퍼만 둔다.
- source: 사용자 요청 — 2026-04-08 core-beliefs 리뷰 세션
- resolution: 처리 2026-04-08 처리 완료.

---

## 2026-04-08 - opencode-review-skill-code-path
- status: done
- suggested-agent: harness-improver
- area: skill
- recommended-artifact: skill
- context-savings: low
- token-meter: pending
- task: `opencode-review` 스킬에 "수정할 코드가 명확한 경우 OpenCode direct-apply 모드를 사용한다"는 라우팅 규칙을 추가한다.
- message: 현재 `opencode-review`는 리뷰 전용이지만, 리뷰 결과로 수정 방향이 명확해지면 바로 OpenCode(`portpolio-implement` 에이전트)에 직접 작성을 위임할 수 있다. 이 경로가 스킬에 명시되지 않아 매번 사용자가 별도로 지시해야 한다. `codex-delegate` SKILL.md의 direct-apply 섹션을 참고해 "review → clear fix → opencode direct-apply" 흐름을 스킬 내 선택 경로로 문서화한다.
- source: 사용자 요청 — 2026-04-08 core-beliefs 리뷰 세션
- resolution: 처리 2026-04-08 처리 완료.

---

## 2026-04-08 - doc-ref-finder-script
- status: done
- suggested-agent: worker
- area: tool
- recommended-artifact: script
- context-savings: high
- token-meter: pending
- task: `tools/find_doc_refs.py <파일경로>` 스크립트를 작성한다 — 마크다운 파일명을 인자로 받아 저장소 전체에서 참조하는 파일 경로와 라인 번호를 출력
- message: doc-value-audit 실행 중 문서 참조 현황 파악에 문서당 Grep 1회 + 라인 확인용 Read가 반복됐다. 이번 감사(10개 문서)에서 약 15~20 툴 호출이 발생. 스크립트가 있으면 `Bash 1회`로 대체 가능. doc-value-audit 스킬의 "링크 참조 현황 확인(`Grep`으로 파일명 검색)" 단계를 이 스크립트 호출로 교체하고 스킬 설명도 업데이트해야 한다.
- source: doc-value-audit 실행 - 2026-04-08 서버 문서 검증
- resolution: 처리 2026-04-08 처리 완료.

---

## 2026-04-14 - document-gitbutler-cli-workflow-as-skill
- status: done
- suggested-agent: harness-improver
- area: skill
- recommended-artifact: skill
- context-savings: medium
- token-meter: pending
- task: GitButler CLI(`but`) 사용 흐름을 별도 문서 + skill로 정리한다.
- message: clang-format 일괄 적용 작업에서 `but stage` → `but commit` → `but push` 루틴 사용 시 hunk lock으로 인한 브랜치 분리 문제가 반복됨. 기본 절차, 잠금 처리 선택지, 바이너리 경로를 스킬로 문서화 필요.
- source: server/ clang-format sweep 세션
- resolution: `.claude/skills/gitbutler/SKILL.md` 추가 (commit 7d3356f). 2026-04-16 처리 완료.

---

## 2026-04-08 - command-script-usage-purpose-log
- status: done
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: medium
- token-meter: pending
- task: bash, PowerShell, Python 등 명령 실행 시 목적과 사용 스크립트를 구조화해 로깅하는 도구 또는 훅을 추가한다.
- message: 사용자가 에이전트가 `bash`, `python`, PowerShell 등으로 명령을 실행할 때 어떤 목적으로 어떤 스크립트나 커맨드를 사용했는지 기록하길 요청했다. 이 로그는 자주 쓰이는 스크립트와 반복 커맨드를 식별해 자동화, 스킬화, 문서 개선 피드백으로 연결하는 데 쓰인다. claim 시에는 기존 `_workspace/token-log.jsonl`, `_workspace/skill-usage-log.jsonl`, `tools/skill_usage_logger.py`, Claude Code hook 설정과 중복되지 않게 설계하고, command, purpose, script_path, cwd, result 정도의 최소 필드를 검토한다.
- source: 사용자 피드백 - 2026-04-08 명령/스크립트 사용 목적 로깅 요청
- resolution: 처리 2026-04-08 처리 완료.
