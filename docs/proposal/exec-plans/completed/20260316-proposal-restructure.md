# 기획서 구조 재정리

## 목표
`docs/proposal/`을 허브 문서와 주제별 세부 문서로 분리하고, 다른 에이전트가 이어받을 수 있는 진행 중 작업 문서 체계를 추가한다.

## 범위
- `docs/proposal/AGENTS.md`
- `docs/proposal/main.md`
- `docs/proposal/exec-plans/active/INDEX.md`
- `docs/proposal/exec-plans/active/20260316-proposal-restructure.md`
- `docs/proposal/common/*.md`
- `docs/proposal/ingame/*.md`
- `docs/proposal/outgame/*.md`
- `docs/proposal/server/*.md`
- 필요 시 `docs/AGENTS.md`
- 필요 시 `docs/workflow/agent-workflow.md`

## 완료 기준
- [x] `docs/proposal/` 전용 `AGENTS.md`가 존재한다
- [x] `docs/proposal/main.md`가 허브 문서 역할만 수행한다
- [x] `docs/proposal/exec-plans/active/` 구조와 인덱스가 추가된다
- [x] 공통/인게임/아웃게임 주요 문서가 동일한 정리 원칙을 따른다
- [x] `server/` 하위 기획 문서도 동일한 정리 원칙으로 맞춘다
- [x] 문서 간 중복 설명이 줄고, 관련 문서 링크가 정리된다

## 진행 상황
- [x] `docs/proposal/` 진입 규칙과 읽기 원칙 추가
- [x] 허브 문서를 핵심 결정/마일스톤/문서 지도로 축약
- [x] proposal 전용 active exec-plan 구조 추가
- [x] 공통 문서 1차 정리 (`common/item.md`, `common/skill.md`, `common/user.md`)
- [x] 인게임 문서 정리 (`ingame/combat.md`, `ingame/dungeon_progress.md`, `ingame/boss_pattern.md`, `ingame/monster_ai.md`)
- [x] 아웃게임 문서 정리 (`outgame/party.md`, `outgame/chat.md`, `outgame/economy.md`, `outgame/dungeon_entry.md`)
- [x] 서버 기획 문서 정리 (`server/architecture.md`, `server/data_driven.md`, `server/performance_test.md`)
- [x] 문서 작업 시 `server/tools/doc_check.py`를 검증 경로에 포함하도록 라우팅 보강
- [x] 문서 검증 스크립트 실행 환경 정상화

## 설계 결정 기록
- `docs/proposal/`은 구현 문서가 아니라 상위 기획 문서 계층으로 유지한다.
- `main.md`에는 핵심 결정과 지도만 두고 세부 규칙은 하위 문서로 분리한다.
- 하위 문서는 가능한 한 `목적 -> 핵심 규칙 -> 주요 흐름/상태 전이 -> 제약/예외 -> 관련 문서` 순서를 따른다.
- DB 테이블, 패킷, 구현 세부는 기획 판단에 꼭 필요할 때만 최소화해서 남기고, 확정 구현 문서는 앱 디렉터리로 넘긴다.
- 1차 정리는 문서 구조 통일과 중복 축소를 우선하고, 세부 균형 조정은 후속 plan 없이 이어서 보완 가능하게 남긴다.
- 이번 누락 원인은 `docs/proposal/` 라우팅 문서에 검증 스크립트 경로가 없었고, 루트 워크플로도 문서 작업 검증 명령을 별도 강조하지 않았기 때문이다.
- 현재 Windows 환경에서는 `python`이 Microsoft Store stub(`C:\Users\park\AppData\Local\Microsoft\WindowsApps\python.exe`)로 잡혀 스크립트 실행이 실패하므로, 문서에는 명령뿐 아니라 실행 전제도 함께 적어야 한다.
- 이후 실제 Python(`C:\Python314\python.exe`)이 PATH에 반영된 뒤 `python server/tools/doc_check.py` 실행이 가능해졌고, 스크립트는 외부/generated 문서 제외 및 영문 exec-plan 헤더 허용까지 보강했다.
