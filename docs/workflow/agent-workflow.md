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

## Skill / Agent 저장 위치

- 에이전트 정의의 **단일 소스**는 루트 `.claude/agents/`다.
- 저장소 공용 skill은 루트 `.claude/skills/` 아래에 생성한다.
- `.codex/agents/`에는 아무것도 생성하지 않는다.
- Codex에서 에이전트를 사용할 때는 `.claude/agents/<name>.md`를 읽어 prompt로 전달한다.
- Codex UI용 메타데이터는 `.codex/skills/<skill>/agents/openai.yaml`에만 둔다.
- 앱 디렉터리(`server/` 등) 아래에는 skill 또는 agent 저장소를 만들지 않는다.

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
- `.codex/agents/`에 에이전트 파일 생성 (`.claude/agents/`가 단일 소스)
