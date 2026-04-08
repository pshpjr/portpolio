# 에이전트 작업 흐름 (빠른 참조)

> 상세 절차는 [agent-workflow-detail.md](./agent-workflow-detail.md)를 참조한다.

## 작업 시작 체크리스트

```
□ AGENTS.md 읽기
□ docs/exec-plans/active/INDEX.md 확인
  → 진행 중인 플랜이 있으면: 해당 파일을 읽고 컨텍스트 복원
  → 없으면: 새 플랜 파일 작성 후 시작
□ 관련 도메인 문서 읽기
□ 문서 작업이면 문서 검증 스크립트 존재 여부 확인
```

## exec-plan 위치 규칙

- 위치: 해당 앱/영역의 `docs/exec-plans/active/`
- 파일명: `YYYYMMDD-작업명.md`
- 완료 시: `docs/exec-plans/completed/`로 이동
- 작성 기준: [exec-plan-template.md](./exec-plan-template.md)

## 브랜치 운영 (기본 순서)

1. 작업 시작 시 자기 도메인의 브랜치로 이동한다.
2. 의미 있는 단위가 나오면 자기 브랜치에 커밋·푸시한다.
3. 주기적으로 최신 `dev`를 merge 해 드리프트를 줄인다.
4. 완료 후 자기 브랜치를 `dev`에 반영하고 `dev`도 푸시한다.

## 완료 후 자동 커밋 (기본 순서)

1. 관련 검사 명령을 모두 통과시킨다.
2. 이번 작업이 만든 변경만 선별 스테이징한다.
3. 비대화형 커밋 메시지로 커밋한다.
4. 현재 작업 브랜치에 푸시하고 최종적으로 `dev`에 반영한다.

## PR 전 필수 검사

```bash
# 전체 검사 한 번에 (권장)
python tools/check_all.py
```

상세 체크리스트 → [pr-checklist.md](./pr-checklist.md)

## 금지 행동

- 레이어 의존성 위반 코드 커밋
- 테스트 없는 Core 레이어 신규 코드
- `check_layers.py` 통과 전 PR 생성
- exec-plan 없이 대형 작업 시작 (300줄 이상 변경 예상 시)
- 문서를 코드보다 나중에 작성
- 문서 변경 후 검증 스크립트 미확인
- `.codex/agents/`에 내용이 있는 에이전트 파일 생성

## 반복 작업 자동화 도구

새 exec-plan 생성 (보일러플레이트 제거):
```bash
python tools/new_exec_plan.py --name <slug> --area docs|proposal|server|lib --goal "<한 줄 목표>"
```

## Codex / OpenCode 위임 기준

다음 조건 중 하나 이상이면 직접 구현 전에 `codex-delegate` 또는 `opencode-review` 스킬 검토:
- 반복 구현 패턴이 이미 존재하거나, 동일 작업을 이전 세션에서 한 번 이상 수행
- 검증 없이 재시도가 2회 이상 예상되는 작업 (첫 시도 실패 가능성 높음)
- 300줄 이상 순수 코드 생성 (문서/주석 제외)
- Claude가 컨텍스트를 많이 소비해야 하는 리뷰 작업

## 피드백 / 개선 큐 / LLM 아티팩트

- 독립 후속 작업 → [harness-improvement-queue.md](./harness-improvement-queue.md)
- 외부 LLM 응답 요약 → `_workspace/agent-notes/*.md` (`record_agent_artifact.py` 사용)
- substantial task → `python tools/context_meter.py --git-base HEAD --files <파일>` 로 계측 후 exec-plan에 기록

## harness-improvement 사용 트리거

작업 중 아래 상황이 발생하면 **반드시** `harness-improvement` 스킬을 호출한다:

- 규칙·문서·프롬프트 빈틈 때문에 우회가 필요했을 때
- 같은 설명이나 생성을 반복해야 했을 때 (스크립트/스킬 후보)
- 작업 완료 후 분리 가능한 후속 정리가 남았을 때
- 다음 세션에서 동일 문제가 반복될 가능성이 보일 때

큐에 항목 등록 시 `task`, `message`, `recommended-artifact`, `context-savings`를 반드시 포함한다.
