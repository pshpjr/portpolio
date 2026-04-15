# 아티팩트 저장 정책

> LLM 응답 아티팩트 저장 규칙과 Skill/Agent 파일 저장 위치 규칙입니다.

---

## LLM 응답 아티팩트

외부 LLM 또는 OpenCode 호출 뒤에는 raw thinking 전체를 저장하지 않는다.
대신 응답 직후의 구조화된 요약만 파일로 남긴다.

기본 경로: `_workspace/agent-notes/*.md`

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

## Skill / Agent 저장 위치

- 에이전트 정의의 **단일 소스**는 루트 `.claude/agents/`다.
- 저장소 공용 skill은 루트 `.claude/skills/` 아래에 생성한다.
- `.codex/agents/`는 `.claude/agents/`의 참조 스텁만 둔다 (내용 복제 금지).
- Codex에서 에이전트를 사용할 때는 `.claude/agents/<name>.md`를 읽어 prompt로 전달한다.
- Codex UI용 메타데이터는 `.codex/skills/<skill>/agents/openai.yaml`에만 둔다.
- 앱 디렉터리(`server/` 등) 아래에는 skill 또는 agent 저장소를 만들지 않는다.
