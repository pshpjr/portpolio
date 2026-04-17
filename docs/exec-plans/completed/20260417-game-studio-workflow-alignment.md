# Exec Plan: Game Studio 워크플로우 정렬

> 생성: 2026-04-17
> 영역: `AGENTS.md`, `docs/AGENTS.md`, `docs/workflow/`

## 목표

저장소에 추가된 `Claude-Code-Game-Studios-main/` 기준으로 기존 루트 워크플로우를 정렬하고, 기존 에이전트/스킬과 게임 스튜디오 방식의 역할 중복을 문서 레벨에서 정리한다.

## 범위

- 루트/문서 진입점(`AGENTS.md`, `docs/AGENTS.md`)에 Game Studio 라우팅 추가
- `docs/workflow/agent-workflow.md`에 Game Studio 실행 모드와 중복 해소 규칙 추가
- Game Studio 방식과 기존 하네스 간 에이전트/스킬 매핑 문서 신설

## 단계

- [x] 1. 현재 워크플로우 진입점과 Game Studio 추가 경로를 문서에서 확인
- [x] 2. 중복 역할(에이전트/스킬) 매핑표를 작성하고 표준 선택 규칙 정의
- [x] 3. AGENTS 및 workflow 문서를 최소 변경으로 업데이트
- [x] 4. 문서 검사(`python tools/doc_check.py`) 실행 (저장소 기존 깨진 링크로 실패, 본 변경과 무관)
- [x] 5. plan 상태를 완료로 갱신하고 인덱스 반영

## 완료 기준

- [x] Game Studio 관련 진입 경로가 AGENTS 문서에서 바로 보인다.
- [x] 기존 하네스와 Game Studio 역할 충돌 시 선택 규칙이 명시된다.
- [x] 중복 에이전트/스킬의 우선 사용 기준이 workflow 문서에 있다.
- [ ] 문서 검사가 통과한다. (기존 저장소 전체 링크 이슈로 미충족)


## 결과 요약

- 루트/문서 AGENTS에 Game Studio 라우팅 링크 추가
- `agent-workflow.md`에 Game Studio 실행 모드와 canonical 소스 규칙 명시
- `game-studio-overlap-map.md` 신설로 중복 agent/skill 선택 기준 고정
- `python tools/doc_check.py`는 기존 저장소 이슈(깨진 링크/기존 exec-plan 형식 경고)로 실패
