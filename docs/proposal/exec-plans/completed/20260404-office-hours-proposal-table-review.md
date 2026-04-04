# Exec Plan: Office-Hours Proposal and Data Table Review

## 목표

`office-hours` 흐름으로 현재 기획서와 데이터 테이블 문서를 함께 검토해,
서버/클라이언트/기획 관점의 공백을 식별하고 원문 문서 보강 또는 pending decision 정리로 수렴한다.

## 범위

- `docs/proposal/main.md`
- `docs/proposal/pending-decisions.md`
- `docs/proposal/common/data-table-conventions.md`
- `docs/proposal/common/item_data_table.md`
- `docs/proposal/ingame/combat_data_tables.md`
- `docs/proposal/ingame/encounter_data_tables.md`
- `docs/proposal/ingame/skill_data_table.md`
- `docs/proposal/reviews/20260404-office-hours-proposal-table-review.md` 신규
- 필요 시 관련 proposal 문서 보강

## 완료 기준

- [x] 서버 관점 검토 결과가 정리된다.
- [x] 클라이언트 관점 검토 결과가 정리된다.
- [x] 기획/범위 관점 검토 결과가 정리된다.
- [x] 데이터 테이블 기준의 핵심 결함과 보강 방향이 문서화된다.
- [x] 필요한 항목이 원문 문서 또는 `pending-decisions.md`에 반영된다.
- [x] `python tools/doc_check.py`가 통과한다.

## 진행 상황

- [x] 관련 proposal active index와 허브 문서를 확인했다.
- [x] 데이터 테이블 문서와 관련 규칙을 읽고 관점별 리뷰를 수집한다.
- [x] 통합 리뷰 문서를 작성한다.
- [x] 필요한 pending decision 또는 원문 보강을 반영한다.
- [x] 문서 검증을 실행하고 기록한다.

## 설계 결정

- 이번 라운드는 구현 코드가 아니라 proposal 문서와 data table authoring 문서를 검토 대상으로 삼는다.
- `office-hours`는 상위 오케스트레이션으로 쓰고, 실제 관점 리뷰는 기획/서버/클라이언트로 나눠 수집한다.
- 데이터 테이블 검토는 단순 포맷 준수보다 "서버/클라이언트가 실제로 필요한 계약을 문서가 약속하는가"를 우선 본다.

## 검증 기록

- `python tools/doc_check.py` → `OK: documentation checks passed.`
- `D:\Users\pshpjr\AppData\Roaming\npm\opencode.cmd run --agent portpolio-review --format json ...` → 제안/데이터 테이블 2차 검토 수행
