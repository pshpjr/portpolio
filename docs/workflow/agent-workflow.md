# 에이전트 작업 흐름 (agent-workflow.md)

## 작업 시작 체크리스트

새 세션에서 작업을 시작할 때:

```
□ AGENTS.md 읽기
□ docs/exec-plans/active/INDEX.md 확인
  → 진행 중인 플랜이 있으면: 해당 파일을 읽고 컨텍스트 복원
  → 없으면: 새 플랜 파일 작성 후 시작
□ 관련 도메인 문서 읽기
□ ARCHITECTURE.md 레이어 규칙 재확인
□ 문서 작업이면 문서 검증 스크립트 존재 여부 확인
```

---

## 실행 계획 파일 (exec-plan)

exec-plan 작성 기준과 템플릿: [exec-plan-template.md](./exec-plan-template.md)

- 위치: 해당 앱/영역의 `docs/exec-plans/active/`
- 파일명: `YYYYMMDD-작업명.md`
- 완료 시: `docs/exec-plans/completed/`로 이동

---

## 브랜치 운영

쓰기 작업을 맡은 에이전트는 공유 브랜치에서 직접 오래 작업하지 않고, 자기 작업 브랜치에서 변경을 관리한다.

기본 순서:

1. 작업 시작 시 자기 브랜치를 만들거나 전환한다.
2. 작업 중간에도 의미 있는 단위가 나오면 자기 브랜치에 커밋하고 푸시한다.
3. 장기 작업 중에는 주기적으로 최신 `dev`를 자기 브랜치에 merge 해서 드리프트를 줄인다.
4. 작업 완료 후 검증이 끝나면 자기 브랜치를 푸시하고, 최종적으로 `dev`에 반영한다.

운영 원칙:

- 브랜치 업데이트는 rebase보다 merge를 기본으로 한다.
- 자기 브랜치에는 자기 작업만 커밋한다.
- `dev` 반영 전에는 관련 검증과 exec-plan 갱신을 먼저 끝낸다.
- merge conflict, 권한 문제, 원격 상태 불일치가 있으면 강행하지 말고 blocker를 보고한다.

## 완료 후 자동 커밋 / 푸시

사용자가 명시적으로 멈추라고 하지 않았다면, 쓰기 작업을 끝낸 에이전트는 검증 통과 후 자기 브랜치 커밋/푸시와 `dev` 반영까지 이어서 수행한다.

핵심 목적:

- 완료된 AI 작업 상태를 대화 바깥의 git 이력으로 남긴다.
- 이후 사용자가 수정을 요청해도, 직전 AI 산출물을 비교 기준으로 다시 확인할 수 있게 한다.
- "일단 끝난 상태"와 "후속 수정 상태"를 서로 다른 커밋으로 분리한다.

기본 순서:

1. 관련 문서/빌드/테스트/검사 명령을 모두 통과시킨다.
2. 이번 작업이 만든 변경만 선별 스테이징한다.
3. 비대화형 커밋 메시지로 커밋한다.
4. 현재 작업 브랜치에 푸시한다.
5. 최신 `dev`를 자기 브랜치에 merge 해 최종 상태를 확인한다.
6. 작업이 끝나면 자기 브랜치를 `dev`에 반영하고 `dev`도 푸시한다.

추적성 규칙:

- 사용자가 나중에 추가 수정이나 방향 전환을 요청할 가능성이 있더라도, 현재 작업이 완료됐으면 먼저 한 번 커밋한다.
- 후속 수정은 이전 완료 커밋 위의 새 커밋으로 쌓는다.
- 사용자가 "이번엔 커밋하지 말라"고 명시한 경우만 예외로 둔다.
- 커밋 없이 여러 완료 상태를 한 워킹트리에 겹쳐 두지 않는다.

안전 조건:

- 현재 작업과 무관한 dirty change는 함께 커밋하지 않는다.
- 검증 실패, push remote 부재, 권한 문제, 안전한 선별 스테이징 불가 상황에서는 무리하게 푸시하지 말고 blocker를 보고한다.
- `dev` merge 또는 `dev` 반영 과정에서 conflict가 나면 자동 해결을 가장하지 말고 충돌 지점을 명확히 보고한다.
- 자동 푸시는 "내가 수정한 파일 집합을 명확히 분리할 수 있을 때"만 수행한다.
- 커밋 전에 관련 exec-plan 진행 상황과 검증 기록을 먼저 갱신한다.
- 커밋 전, 이번 작업에서 재발 가능한 하네스 결함이 드러났다면 `harness-feedback-log.md` 또는 `harness-improvement-queue.md` 갱신이 필요한지 확인한다.

## Skill / Agent 저장 위치

- 에이전트 정의의 **단일 소스**는 루트 `.claude/agents/`다.
- 저장소 공용 skill은 루트 `.claude/skills/` 아래에 생성한다.
- `.codex/agents/`는 `.claude/agents/`의 참조 스텁만 둔다 (내용 복제 금지).
- Codex에서 에이전트를 사용할 때는 `.claude/agents/<name>.md`를 읽어 prompt로 전달한다.
- Codex UI용 메타데이터는 `.codex/skills/<skill>/agents/openai.yaml`에만 둔다.
- 앱 디렉터리(`server/` 등) 아래에는 skill 또는 agent 저장소를 만들지 않는다.

---

## OpenCode 리뷰 / 구현 보조

프로젝트 레벨 OpenCode 설정(`opencode.json`, `.opencode/agents/`)을 통해 로컬 OpenCode CLI를 헤드리스 실행 경로로 활용한다.

**사용 시점:**

- 큰 PR 또는 아키텍처 변경 전 독립적 검토가 필요할 때
- Claude의 1차 리뷰 후 2nd-opinion이 필요할 때
- 기획 결정의 논리적 결함을 다각도로 확인할 때
- OpenCode가 직접 읽기/수정 작업을 맡아도 되는 별도 write scope가 있을 때

**호출 방법:**

```
서브에이전트: opencode-reviewer
스킬: opencode-review
기본 리뷰 호출: opencode run --agent portpolio-review
제안 전용 구현 호출: opencode run --agent portpolio-propose
직접 수정 호출: opencode run --agent portpolio-implement
```

**결과 통합 원칙:**

- OpenCode와 Claude 결과가 일치 → 높은 신뢰로 결론
- OpenCode만 발견 → 반드시 재검토
- 상충 → 사용자에게 두 관점 전달 후 판단 요청
- OpenCode CLI 미설치 또는 실행 실패 시 → 건너뛰고 이유 보고 (강행 금지)
- `portpolio-implement`는 다른 실행자와 같은 write scope를 공유하지 않는다

---

## Gstack 적응 규칙

`gstack`를 참고해 하네스를 확장할 때는 웹 제품 전제를 그대로 들여오지 않는다.

- 유지: `office-hours`, `plan-ceo-review`, `plan-eng-review`, `architecture-eval`, `performance-analysis`, `tradeoff-resolution`, 독립 리뷰, 회고
- 폐기: 브라우저 QA, 웹 배포, CDP/쿠키/스크린샷 흐름
- 세부 운영은 [gstack-office-hours-loop.md](./gstack-office-hours-loop.md)를 따른다
- GLM 교차 검토 슬롯은 기존 `opencode-review`와 `opencode.json` 설정을 사용한다

---

## PR 작성 기준

PR을 열기 전 반드시:

```bash
# 1. 문서 링크/plan 검사
python tools/doc_check.py

# 2. 인코딩 검사
python tools/check_encoding.py

# 3. 서버 의존관계 검사
python tools/check_layers.py --root server/src --layer-order types,config,core,service,network,runtime

# 4. 라이브러리 의존관계 검사
python tools/check_layers.py --root Lib --layer-order include,src,tests
```

- 앱별 빌드와 테스트 명령은 각 앱의 workflow 문서를 따른다.
- 공용 Python 검사 도구 규칙은 [../conventions/tooling.md](../conventions/tooling.md)를 따른다.
- 저장소 텍스트 파일은 UTF-8 without BOM으로 유지한다.

PR 설명 필수 포함:
- **무엇을 했나**: 변경 내용 요약
- **왜 했나**: 동기, 설계 결정
- **영향 범위**: 수정된 레이어/도메인
- **테스트**: 어떻게 검증했나

---

## 피드백 로그

사용자 피드백, 요청된 수정, 구현 중 발생한 프로세스/도구 이슈 중 재발 가능성이 있는 항목은 [harness-feedback-log.md](./harness-feedback-log.md)에 짧게 남긴다.

전체 대화를 옮기지 말고, 나중에 개선 액션으로 이어질 핵심만 기록한다.

권장 기록 시점:
- 사용자가 작업 방식이나 응답 방식 수정을 요구했을 때
- 문서/도구/프롬프트의 빈틈 때문에 작업이 막혔을 때
- 반복될 가능성이 높은 혼선이나 우회가 발생했을 때

각 기록은 `kind`, `area`, `summary`, `impact`, `suggested-follow-up`를 포함한다.

피드백 로그는 "기억할 사건"을 남기는 곳이다. 바로 집어가서 처리할 독립 후속 작업은 [harness-improvement-queue.md](./harness-improvement-queue.md)에 별도로 남긴다.

---

## 재귀 개선 큐

작업 중 또는 작업 종료 시, 하네스의 빈틈이 드러났고 그 문제가 현재 메인 작업과 분리 가능한 후속 작업이라면 [harness-improvement-queue.md](./harness-improvement-queue.md)에 항목을 추가한다.

큐 항목 최소 형식:

```markdown
- recommended-artifact: script | skill | prompt | doc | other
- context-savings: low | medium | high | unknown
- token-meter: `<estimated tokens / changed lines>` | pending
- task: 다음 에이전트가 수행할 작업
- message: 왜 필요한지와 어디서 드러났는지
```

운영 규칙:

- 반복 설명이나 반복 생성 때문에 비효율이 생겼다면 `recommended-artifact`에 `script` 또는 `skill` 후보를 적어 둔다.
- 스크립트/스킬화가 컨텍스트나 토큰 사용량을 줄일 것 같다면 `context-savings`도 함께 남긴다.
- substantial task라면 `python tools/context_meter.py --git-base HEAD --files <읽은 주요 컨텍스트 파일들>`로 작업 분량 대비 컨텍스트 부하를 계측하고, 결과를 exec-plan 또는 큐 항목에 남긴다.
- `task`는 명령형으로 짧게 쓴다.
- `message`는 다음 에이전트가 전체 대화를 다시 읽지 않아도 될 정도의 맥락만 남긴다.
- 현재 작업자가 바로 해결 가능한 항목은 큐에 미루지 말고 지금 처리한다.
- 후속 작업이 독립적이면 `harness-improver`가 claim 해서 처리한다.
- 회고 가치만 있고 즉시 작업으로 쪼개기 어려운 항목은 큐 대신 피드백 로그에 남긴다.

---

## LLM 응답 아티팩트

외부 LLM 또는 OpenCode 호출 뒤에는 raw thinking 전체를 저장하지 않는다.
대신 응답 직후의 구조화된 요약만 파일로 남긴다.

기본 경로:

- `_workspace/agent-notes/*.md`

기본 도구:

```bash
python tools/record_agent_artifact.py --agent <agent> --summary "<short summary>" --source "<task or command>"
```

후속 작업을 큐에 자동 추가하려면:

```bash
python tools/record_agent_artifact.py \
  --agent <agent> \
  --summary "<short summary>" \
  --source "<task or command>" \
  --task "<follow-up task>" \
  --message "<follow-up context>" \
  --recommended-artifact script \
  --context-savings medium
```

운영 규칙:

- 저장 대상은 구조화된 요약, 결정, 후속 작업, 토큰 계측 결과다.
- raw private thinking, 장문 내부 추론, 모델의 숨은 scratchpad는 파일에 남기지 않는다.
- 자동 큐 추가는 현재 작업과 분리 가능한 후속 작업일 때만 사용한다.

---

## 에이전트가 막혔을 때

1. "더 열심히 시도"하지 않는다
2. 무엇이 빠진지 분석:
   - 문서가 부족한가? → `docs/`에 추가
   - 컨벤션이 불명확한가? → `docs/conventions/`에 추가
   - 린터가 없어 실수를 반복하는가? → `tools/`에 검사 추가
3. 부족한 것을 채운 후 다시 시도

---

## 금지 행동

- 레이어 의존성 위반 코드 커밋
- 테스트 없는 Core 레이어 신규 코드
- `check_layers.py` 통과 전 PR 생성
- exec-plan 없이 대형 작업 시작 (300줄 이상 변경 예상 시)
- 문서를 코드보다 나중에 작성 (설계 결정은 코딩 전 또는 동시에)
- 문서 검증 스크립트가 있는데도 문서 변경 후 확인하지 않기
- `.codex/agents/`에 내용이 있는 에이전트 파일 생성 (참조 스텁만 허용, `.claude/agents/`가 단일 소스)
