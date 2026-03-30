# Exec Plan: Client Unreal Gitignore

## 목표

- 루트 Git 저장소에서 `client/` 언리얼 프로젝트의 재생성 가능 산출물을 무시하도록 `.gitignore`를 보완한다.
- 소스 자산과 설정은 유지하고, 빌드/캐시/로컬 사용자 산출물만 제외한다.

## 범위

- 루트 설정:
  - `.gitignore`
- 루트 계획 문서:
  - `docs/exec-plans/active/INDEX.md`
  - `docs/exec-plans/active/20260330-client-unreal-gitignore.md`

## 완료 기준

- [x] 현재 루트 `.gitignore`에 `client/` 언리얼 프로젝트용 규칙 유무를 확인한다.
- [x] `client/DerivedDataCache/`, `client/Intermediate/`, `client/Saved/` 등 재생성 가능한 산출물이 무시된다.
- [x] IDE/솔루션 부산물 중 저장소에 둘 필요가 없는 `client/.vs/`, `client/*.sln`, `client/.vsconfig`가 무시된다.
- [x] `Config/`, `Content/`, `Source/`, `*.uproject` 같은 추적 대상은 무시 규칙에 포함하지 않는다.

## 진행 상황

- [x] 루트/문서 진입점 확인
- [x] active exec-plan 작성 및 인덱스 등록
- [x] 루트 `.gitignore`에 언리얼 클라이언트 규칙 추가
- [x] 변경 검토

## 설계 결정 기록

- 이 작업은 앱 코드 변경보다 루트 저장소 추적 정책 조정이므로 루트 `docs/exec-plans/active/`에서 관리한다.
- 이미 루트 `.gitignore`가 광범위한 목록을 가지고 있어, 충돌을 줄이기 위해 파일 앞부분에 `client/` 한정 규칙을 별도 섹션으로 추가한다.
- 언리얼에서 자주 재생성되는 디렉터리와 로컬 Visual Studio 산출물만 무시하고, 프로젝트 파일과 실제 에셋은 유지한다.
- `git check-ignore -v`로 `client/.vs`, `client/DerivedDataCache`, `client/Intermediate`, `client/Saved`, `client/client.sln`, `client/.vsconfig`가 무시되고 `Config`, `Content`, `Source`, `client.uproject`는 유지됨을 확인했다.
