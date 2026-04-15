# Queue Item 템플릿

`docs/exec-plans/queued/`에 새 항목을 만들 때 이 템플릿을 복사한다.

- 파일명: `YYYYMMDD-작업명.md`
- claim 시: `active/`로 이동하고 full exec-plan으로 확장한다

---

```markdown
# Queue Item: {작업명}

> 등록: YYYY-MM-DD
> 영역: server | proposal | docs | client | lib | harness
> 우선순위: P1 | P2 | P3

## 요약

이 작업이 무엇인지 한두 문장.

## 큐 등록 이유

지금 active가 아닌 이유. 현재 어떤 active 플랜이 우선인지.

## 예상 범위

- 수정/생성할 파일 또는 영역 (대략)
- 예상 크기: 소 / 중 / 대

## claim 시 참고

claim하는 에이전트가 알아야 할 선행 조건이나 맥락.
없으면 이 섹션 삭제.
```

---

## 작성 원칙

- queue item은 가볍게 유지한다. 상세 설계는 claim 후 active exec-plan에서 한다.
- **요약**은 나중에 읽어도 context 없이 이해할 수 있게 쓴다.
- **큐 등록 이유**는 "지금 왜 안 하는지"를 남긴다 — 이것이 없으면 priority 판단이 어렵다.
- **예상 범위**는 claim 시 exec-plan 크기를 가늠하는 용도다. 틀려도 괜찮다.
