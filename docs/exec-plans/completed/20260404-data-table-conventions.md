# Exec Plan: 데이터 테이블 문서 규칙 정립

## 목표

기획 데이터 테이블 문서의 공통 작성 기준을 만들고,
관련 검증 에이전트와 대표 문서를 그 기준에 맞춰 정리한다.

- **작성일**: 2026-04-04
- **범위**: `docs/proposal/common/data-table-conventions.md` 신규 작성, 하네스 참조 연결, SkillSetTable 결함 수정
- **담당**: design-author, 루트 조율자

## 배경

기존 데이터 테이블 문서(combat_data_tables, skill_data_table, item_data_table, encounter_data_tables)가 형식이 제각각이고, 아래 사항이 누락되어 있음:
- 테이블 존재 의도와 런타임 역할 설명 없음
- 복합 PK 사용 기준 없음
- 배열 컬럼 vs 별도 테이블 분리 기준 없음
- 정규화 판단 기준 없음
- 테이블 간 관계 다이어그램 불통일

추가로 `SkillSetTable`이 기획 의도(무기당 복수 스킬, 플레이어가 슬롯 배정 선택)를 반영하지 못함.

## 작업 목록

- [x] `docs/exec-plans/active/20260404-data-table-conventions.md` 작성
- [x] `docs/exec-plans/active/INDEX.md` 갱신
- [x] `docs/proposal/common/data-table-conventions.md` 작성
- [x] `.claude/agents/data-table-reviewer.md` 작성 (OpenCode 2차 검증 포함)
- [x] `docs/proposal/ingame/skill_data_table.md` — SkillSetTable → WeaponDefaultSkillTable 재정의, CharacterSkillUnlockTable / CharacterSkillPresetTable 추가
- [x] `design-author.md` 테이블 규칙 및 완료 체크리스트 추가
- [x] `docs/proposal/AGENTS.md` 데이터 테이블 문서 규칙 섹션 추가
- [x] `docs/proposal/main.md` data-table-conventions 링크 추가

## 완료 기준

- 모든 신규 데이터 테이블 문서가 data-table-conventions.md 형식을 따를 수 있는 기준 존재
- data-table-reviewer 에이전트가 conventions 기준으로 검증 가능
- skill_data_table.md가 기획서(skill.md)의 "복수 스킬 해금 + 슬롯 배정" 구조를 반영
