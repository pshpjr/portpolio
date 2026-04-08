# Queued Exec Plans

> 현재 실행하지 않고 대기 중인 작업 목록입니다.
> 에이전트는 active 플랜이 완료되거나 줄어들 때 여기서 pop합니다.

## 대기 중인 작업

_(없음)_

## 운영 규칙

- 에이전트가 큐에 넣을지 판단 → 사용자에게 확인 → 승인 시 이 파일에 추가
- claim 시: 해당 파일을 `active/`로 이동 + 이 INDEX에서 항목 제거 + `active/INDEX.md`에 추가
- 우선순위는 P1(긴급) / P2(보통) / P3(낮음)으로 표시
- 같은 영역의 active 플랜이 없어지면 P1부터 claim

## 항목 형식

```
- [YYYYMMDD-slug.md](./YYYYMMDD-slug.md) — 영역: server|proposal|docs|client|lib|harness | 우선순위: P1|P2|P3 | 한 줄 요약
```
