# Exec Plan: Monorepo Agent Doc Alignment

## 목표

- 모노레포 구조에 맞게 `AGENTS.md`와 `context-map` 라우팅 오류를 바로잡는다.
- 서버와 `Lib/`가 함께 따를 공용 C++ 컨벤션, 설계 철학, 검사 도구의 상위 위치를 정한다.
- 문서 검사와 의존관계 검사 도구를 앱 하위가 아니라 공용 계층으로 옮기고, 서버와 `Lib/`가 이를 사용하도록 정리한다.

## 범위

- 루트/상위 문서:
  - `docs/AGENTS.md`
  - `docs/workflow/agent-workflow.md`
  - 필요 시 공용 컨벤션/철학/검사 문서 신규 작성
- 서버 문서:
  - `server/AGENTS.md`
  - `server/docs/context-map.md`
  - `server/docs/INDEX.md`
  - 필요 시 서버 전용 문서의 공용 문서 참조 정리
- 라이브러리 문서:
  - `Lib/AGENTS.md`
  - 필요 시 `Lib/README.md`
  - 필요 시 `Lib/docs/exec-plans/active/INDEX.md`
- 공용 검사 도구:
  - `tools/check_layers.py`
  - `tools/doc_check.py`
  - `tools/check_encoding.py`
  - 기존 `server/tools/` 호환 래퍼 정리
  - 필요 시 관련 사용 문서

## 완료 기준

- [x] `AGENTS.md`와 `context-map`의 잘못된 라우팅 또는 존재하지 않는 로컬 문서 참조가 정리된다.
- [x] 서버와 `Lib/`가 공통으로 따를 C++ 컨벤션/설계 철학 문서가 상위 계층에 배치된다.
- [x] 서버와 `Lib/`의 진입 문서가 상위 공용 문서를 올바르게 참조한다.
- [x] 문서 검사 도구가 `Lib/` 문서를 검사 대상으로 포함하고 공용 위치에서 실행 가능하다.
- [x] 의존관계 검사 도구가 `Lib/`에도 적용 가능한 방식으로 공용 위치로 정리되거나, 공용 입력 구조를 지원한다.
- [x] 변경된 문서와 도구를 기준으로 관련 검증 명령을 실행해 결과를 확인한다.

## 진행 상황

- [x] 관련 진입 문서와 검사 도구 구조 조사
- [x] exec-plan 작성 및 범위 확정
- [x] 문서 라우팅 구조 수정
- [x] 공용 컨벤션/설계 철학 문서 계층 재배치
- [x] 공용 검사 도구 계층 재배치
- [x] 서버/`Lib/` 참조 구조 정리
- [x] 검증 실행

## 설계 결정 기록

- 이 작업은 특정 앱 구현보다 저장소 전역 문서/검사 체계 정리에 가깝기 때문에 루트 `docs/exec-plans/active/`에서 관리한다.
- 서버와 `Lib/`가 공통으로 따르는 기준은 특정 앱 하위가 아니라 루트 문서 계층으로 승격한다.
- 공용 검사 도구도 특정 앱 하위보다 루트 공용 위치가 우선이며, 문서와 실행 경로를 그 기준에 맞춘다.
- 서버 전용 문서에는 서버 고유 제약만 남기고, 공용 규칙은 상위 문서로 이동한 뒤 링크로 참조한다.
- 검증 결과 공용 `doc_check.py`, `check_layers.py`, `check_encoding.py`는 모두 실행되었고, `Lib/docs/exec-plans/active/20260317-utility-functions.md`의 기존 완료 기준 누락 경고만 별도로 남아 있다.
