# Exec Plan: Office Hours — 기획서 폴리싱 및 데이터 테이블 검토

- **작성일**: 2026-04-04
- **범위**: `docs/proposal/**` 전체, 데이터 테이블 문서
- **담당**: office-hours-orchestrator, design-author, data-table-reviewer

## 목적

이전 review 라운드(full-proposal-completion, data-table-conventions) 이후 남은 미흡 사항을 집중 분석하고, 기획서 폴리싱과 데이터 테이블 구조 검증을 추가로 실행한다.

## 작업 목록

- [x] 1. proposal/main.md + 주요 하위 문서 현황 파악
- [x] 2. 기존 리뷰 라운드(20260404-office-hours-proposal-table-review) 결과 활용
- [x] 3. item_data_table.md — conventions 포맷 리라이트 + DungeonRewardProfileTable 추가 + LockReasonTag 추가
- [x] 4. encounter_data_tables.md — conventions 포맷 리라이트 + BossCombatProfileTable/BossStaggerRuleTable 추가 + HUD key 보강
- [x] 5. combat_data_tables.md — 관계 다이어그램 + 의도 한 문장 추가
- [x] 6. pending-decisions.md — 10개 항목 이미 정리됨 확인
- [x] 7. opencode-review 최종 크로스체크 — Claude + OpenCode 교차 검토 완료
  - item_data_table.md line 237 stale 문구 수정 (pending #4 확정 반영)
  - combat_data_tables.md WeaponTable 컬럼 중복 교차 참조 주석 추가 (pending #6)
  - combat_data_tables.md "서버 구현으로 넘기는 항목" 섹션 추가 (임시 버프 authoring 누락 해소)
  - data-table-conventions.md schema.json 위임 예외 규칙 추가
  - LOW 이슈 5건 (SkillTripoTable 명칭, DilTimeSec 표기 등)은 pending-decisions에 추가하지 않음 — v1 범위 영향 없음
- [x] 8. 완료 처리

## 완료 기준

- 데이터 테이블 문서가 data-table-conventions.md 기준을 통과
- 기획서 주요 미흡점이 문서에 반영되거나 pending-decisions.md로 격리
- OpenCode 크로스체크 통과
