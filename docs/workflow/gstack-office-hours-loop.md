# Gstack Office-Hours Loop for Portpolio

`gstack`의 상위 사고 루프만 가져와 `portpolio`의 문서 우선 게임 기획 흐름에 맞춘 운영 문서다.

## 목표

- 사용자가 office-hours처럼 아이디어만 던져도 하네스가 기획 라운드를 시작한다.
- CEO 관점, 엔지니어링 관점, 아키텍처, 성능, 트레이드오프 분석을 서로 분리된 스킬로 수행한다.
- 서버/클라이언트가 필요한 데이터와 상태 계약을 기획 문서에 역으로 요구한다.
- 마지막에 `OpenCode(GLM)`로 2차 검토해 사각지대를 줄인다.

## 유지할 것

- `office-hours`: 모호한 요구를 작업 가능한 기획 입력으로 정리하는 시작점
- `plan-ceo-review`: 범위, 우선순위, 포트폴리오 가치, 무엇을 버릴지 검토
- `plan-eng-review`: 서버/클라/데이터 계약, 상태 전이, 실패 흐름 검토
- `architecture-eval`: 구조 후보 비교, 선택 근거 정리, 리스크 평가
- `performance-analysis`: 병목 가설, 측정 항목, 수용 기준, 검증 방법 정리
- `tradeoff-resolution`: 상충하는 선택지 정리와 해소안 제시
- `review` 계열 2차 검토: `opencode-review`를 통한 GLM 교차 검토
- `retro`: 반복 이슈를 workflow/log에 환원

## 버릴 것

- 브라우저 연결, CDP, 쿠키, 스크린샷, 웹 E2E 전제
- `/qa`, `/qa-only` 같은 웹 UI 검증 흐름
- `/ship`, `/land-and-deploy` 같은 배포 전제 흐름
- Supabase, 웹앱 런타임, 페이지 라우팅 중심 산출물

## 분리된 스킬 구조

### `office-hours`

- 가장 상위 오케스트레이터
- 사용자 입력을 기획 범위, 선행 문서, 필요한 검토 종류로 분해
- 필요 시 아래 스킬들을 순차적으로 호출

### `plan-ceo-review`

- 포트폴리오 가치, 범위, 우선순위, 데모 적합성, 과잉 약속 여부 검토
- 기존 `portfolio-scope-guardian`, 필요 시 기존 기획 비평 에이전트를 함께 활용

### `plan-eng-review`

- 서버/클라이언트 구현 관점에서 빠진 데이터와 상태 계약 검토
- `server-proposal-critic`, `client-proposal-critic` 활용

### `architecture-eval`

- 후보 구조 2개 이상을 비교하고 선택 이유를 남김
- 레이어 경계, 책임 분리, 변경 비용, 테스트 가능성 평가

### `performance-analysis`

- 어떤 지표를 측정해야 하는지, 어디가 병목 후보인지, 어떤 부하 시나리오로 검증할지 정의
- "빠를 것 같다"가 아니라 계측 계획과 수용 기준을 산출

### `tradeoff-resolution`

- 충돌하는 선택지를 표로 정리
- 각 옵션의 비용, 이득, 리스크, 완화책, 보류 조건을 문서화

## 권장 실행 순서

1. `office-hours`
2. `plan-ceo-review`
3. `plan-eng-review`
4. 필요 시 `architecture-eval`
5. 필요 시 `performance-analysis`
6. 선택 충돌이 있으면 `tradeoff-resolution`
7. 최종 `opencode-review`

## 산출물 규칙

- 원문 기획 문서: `docs/proposal/**`
- 미결 항목: `docs/proposal/pending-decisions.md`
- 별도 검토 기록: `docs/proposal/reviews/YYYYMMDD-*.md`
- 루트 운영 판단과 하네스 규칙: `docs/workflow/**`

## 운영 원칙

- 리뷰 문서만 늘리고 원문 기획을 보강하지 않는 상태를 허용하지 않는다.
- 서버/클라 비판은 반드시 "기획 문서가 추가로 약속해야 하는 것"으로 환원한다.
- 아키텍처와 성능 분석은 `plan-eng-review`의 하위 체크리스트가 아니라 독립 재실행 가능한 분석으로 분리한다.
- 트레이드오프는 최종 선택만 적지 말고, 왜 다른 옵션을 버렸는지도 남긴다.
- GLM 결과는 최종 판결이 아니라 2차 경고 장치다.
