# 에이전트 작업 흐름 (상세 절차)

> 빠른 참조는 [agent-workflow.md](./agent-workflow.md)를 먼저 읽는다.

---

## 공유 브랜치 작업 원칙

- 공유 브랜치에서 직접 오래 작업하지 않는다. 자기 작업 브랜치에서 변경을 관리한다.
- merge conflict, 권한 문제, 원격 상태 불일치가 있으면 강행하지 말고 blocker를 보고한다.

---

## 완료 상태 추적 원칙

- 현재 작업이 완료됐으면 먼저 커밋한다. 후속 수정은 그 위에 새 커밋으로 쌓는다.
- 현재 작업과 무관한 dirty change는 함께 커밋하지 않는다.
- 검증 실패, push 불가, 선별 스테이징 불가 상황에서는 blocker를 보고한다.

---

## 에이전트가 막혔을 때

1. "더 열심히 시도"하지 않는다.
2. 무엇이 빠진지 분석한다:
   - 문서가 부족한가? → `docs/`에 추가
   - 컨벤션이 불명확한가? → `docs/conventions/`에 추가
   - 린터가 없어 실수를 반복하는가? → `tools/`에 검사 추가
3. 부족한 것을 채운 후 다시 시도한다.
4. 재발 가능한 빈틈이면 `harness-improvement-queue.md`에 항목을 등록한다.

---

## 운영 정책 참조

상황별 상세 규칙은 아래 문서에서 찾는다.

| 주제 | 문서 |
|---|---|
| 브랜치/merge 정책, 자동 커밋/푸시 | [vcs-policy.md](./vcs-policy.md) |
| PR 제출 전 체크리스트 | [pr-checklist.md](./pr-checklist.md) |
| LLM 아티팩트 저장, skill/agent 저장 위치 | [artifact-policy.md](./artifact-policy.md) |
| 하네스 개선 큐 운영 규칙 | [harness-improvement-queue.md](./harness-improvement-queue.md) |
| OpenCode 검토/구현 보조 | `opencode-review` 스킬 |
