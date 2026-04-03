# Exec Plan: 전체 기획서 완성 (Full Proposal Completion)

> 생성: 2026-04-03
> 영역: `docs/proposal/`

## 목적

기존 기획서 문서 전체를 분석하고, 전문 review agent(gameplay-loop-critic, co-op-flow-advocate, economy-skeptic, fantasy-curator, portfolio-guardian)의 피드백을 수집한 뒤, design-author가 문서를 보강·완성한다. 사용자가 결정해야 할 항목은 별도 문서로 통합한다.

## 목표

- 다중 에이전트와 다중 모델 리뷰를 반복 실행할 수 있는 작업 기반을 만든다.
- proposal 허브와 세부 문서에 v1 컷라인, 무기 전환, UI/카메라/HUD, 협동 복구 흐름을 반영한다.
- 사용자 결정이 필요한 항목을 `docs/proposal/pending-decisions.md`에 모은다.

## 단계

- [x] 1. 5개 review agent 병렬 실행 → 기획서 전체 분석 및 피드백 수집
- [x] 2. 피드백 통합 → `docs/proposal/pending-decisions.md` 생성 (사용자 결정 필요 항목)
- [x] 3. design-author → 기획서 문서 보강·완성 (확정 가능한 항목만)
- [x] 4. main.md 하위 문서 지도 갱신
- [ ] 5. 완료 처리

## 영향 경로

- `docs/proposal/**/*.md` (전체)
- `docs/proposal/pending-decisions.md` (신규)
- `tools/proposal_review_round.py` (반복 리뷰 자동화)

## 완료 기준

- [x] 반복 리뷰 스크립트로 최소 2회 이상 리뷰 라운드를 실행한다.
- [x] proposal 핵심 문서에 v1 컷라인과 UI/카메라/HUD 규칙이 반영된다.
- [x] `pending-decisions.md`에 남은 사용자 결정 항목이 정리된다.
- [x] 문서 검증을 다시 통과한다.

## 운영 규칙

- Claude 메인 컨텍스트는 최소 사용. review/작성은 subagent에 위임.
- 확정되지 않은 수치/방향 결정은 pending-decisions.md에만 기록.
- 기존 결정을 뒤집는 수정은 pending-decisions.md로 격리.

## 진행 메모

- review 관점: gameplay, co-op, economy/progression, portfolio scope, fantasy theme 5축을 병렬 수집했다.
- 추가 사용자 결정: `무기 변경 가능`은 확정 사항으로 반영한다.
- 추가 보강 요구: UI, 화면 구성, 카메라 거리 같은 핵심 플레이 표현 요소도 proposal에 포함한다.
- OpenCode 기반 리뷰 자동화 스크립트를 `tools/proposal_review_round.py`에 추가했다.
- 자동 리뷰 라운드 1, 2, 3을 `docs/proposal/reviews/`에 기록했다.
- 라운드 3 기준 `Readiness / Combat / Systems / Co-op` 4축이 모두 `COMPLETE` 판정을 반환했다.
