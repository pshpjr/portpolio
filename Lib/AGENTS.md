# AGENTS.md
> `Lib/` 공유 라이브러리 영역의 진입 문서입니다.
> 서버와 언리얼 클라이언트가 함께 사용할 수 있는 공용 코드만 이 디렉터리에 둡니다.

---

## 목적

- `server/`와 향후 `client/`(Unreal)에서 함께 사용할 수 있는 타입, 유틸, 프로토콜 보조 코드 관리
- 특정 런타임에 종속되지 않는 재사용 가능한 라이브러리 우선 배치

## 배치 원칙

- 서버 전용 구현은 `server/`에 둡니다.
- 언리얼 엔진 전용 구현은 `client/`에 둡니다.
- 공용으로 유지할 수 있는 헤더, 데이터 구조, 순수 로직, 직렬화 보조 코드는 `Lib/`에 둡니다.
- 엔진/플랫폼 의존성이 생기면 `Lib/`에 두지 말고 각 앱으로 되돌립니다.

## 작업 시작 게이트

Lib의 모든 작업은 크기와 무관하게 exec-plan 작성으로 시작한다.

1. [docs/INDEX.md](./docs/INDEX.md)에서 필요한 문서를 고른다
2. [docs/exec-plans/active/INDEX.md](./docs/exec-plans/active/INDEX.md)에서 관련 active plan 확인
3. 관련 plan이 없으면 새 exec-plan을 먼저 작성하고 INDEX.md에 등록
4. exec-plan을 사용자에게 검토받기 전에는 코드, 빌드 설정, 테스트를 수정하지 않는다
5. 변경 대상이 정말 공용 코드인지 확인
6. 서버 또는 언리얼 전용 의존성이 없는지 점검
7. 필요하면 `README.md`에 폴더 구조와 사용 위치를 함께 갱신

## 네임스페이스 규칙

`Lib/`의 모든 코드는 `portpolio::lib` 하위 네임스페이스를 사용한다.

| 분류 | 네임스페이스 |
|------|-------------|
| 공용 타입, 타입 안전 래퍼 | `portpolio::lib::types` |
| 공용 유틸 함수 | `portpolio::lib::utils` |
| 직렬화/프로토콜 보조 | `portpolio::lib::proto` |

- 서버의 `portpolio::types`와 혼용하지 않는다.
- 네임스페이스가 추가될 때 이 표에 함께 등록한다.

## 현재 기준

- 공유 라이브러리 개요: [README.md](./README.md)
- 작업 유형별 문서: [docs/INDEX.md](./docs/INDEX.md)
- 경로별 최소 문서: [docs/context-map.md](./docs/context-map.md)
- 활성 실행 계획: [docs/exec-plans/active/INDEX.md](./docs/exec-plans/active/INDEX.md)
- 공용 설계 철학: [../docs/design/core-beliefs.md](../docs/design/core-beliefs.md)
- 공용 C++ 규칙: [../docs/conventions/cpp-style.md](../docs/conventions/cpp-style.md)
- 공용 Python 도구 규칙: [../docs/conventions/tooling.md](../docs/conventions/tooling.md)
- 서버 전용 규칙: [../server/AGENTS.md](../server/AGENTS.md)
- 상위 문서/워크플로 규칙: [../docs/AGENTS.md](../docs/AGENTS.md)
