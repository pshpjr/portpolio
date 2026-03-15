# 프로젝트 초기 설정

## 목표
문서 구조 정리와 빌드 가능한 CMake 스캐폴딩, 레이어 검사 툴을 갖춘 초기 상태 구성.

## 범위
- CMakeLists.txt
- vcpkg.json (manifest 모드 의존성 선언)
- CMakePresets.json (debug / release / ci 프리셋)
- src/ 디렉토리 구조 + 플레이스홀더
- tests/ 기본 구성 (GoogleTest)
- tools/check_layers.py 구현
- tools/doc_check.py 정비
- tools/requirements.txt
- .clang-format
- .github/workflows/ci.yml (빌드 + 테스트 + 레이어 검사)

## 완료 기준
- [ ] `cmake --preset debug` 성공 (vcpkg 패키지 설치 포함)
- [ ] `cmake --build --preset debug` 성공
- [ ] `ctest --preset debug` 성공 (빈 테스트 스위트도 통과)
- [ ] `python tools/check_layers.py` 실행 성공 (위반 0건)
- [ ] `python tools/doc_check.py` 실행 성공
- [ ] GitHub Actions CI 파이프라인 녹색
- [ ] 모든 레이어 디렉토리 존재 (빈 파일이라도)

## 진행 상황
- [x] vcpkg.json 작성 (spdlog, flatbuffers, asio, nlohmann-json, gtest)
- [x] CMakePresets.json 작성 (debug / release / ci)
- [ ] 문서 재배치 및 링크 정리
- [ ] CMakeLists.txt 작성
- [ ] 레이어 디렉토리 구조 생성
- [ ] 검사 툴 정리
- [ ] GoogleTest 연동
- [ ] CI 워크플로우 작성

## 설계 결정 기록
- 빌드 시스템: CMake 선택 (크로스 플랫폼, 업계 표준)
- 패키지 관리: vcpkg manifest 모드 채택 (`vcpkg.json` + `CMakePresets.json`)
  - `VCPKG_ROOT` 환경 변수로 툴체인 경로 주입 → 로컬/CI 모두 동일한 방식
  - 향후 서드파티 라이브러리 추가 시 `vcpkg.json`의 `dependencies` 배열에 항목만 추가
  - FetchContent 미사용: 버전 고정 및 재현성 확보를 위해 vcpkg로 일원화
- CI: GitHub Actions (무료 플랜)
## 2026-03-14 검증 업데이트

- `VCPKG_ROOT=C:\vcpkg` 기준으로 `cmake --preset debug`, `cmake --build --preset debug`, `ctest --preset debug` 재검증 성공.
- `vcpkg.json` 은 manifest 모드 재현성을 위해 `builtin-baseline` 을 포함해야 했다. 누락 시 configure 단계에서 vcpkg install 이 실패했다.
- Windows 에서는 일반 PowerShell보다 Visual Studio Developer Command Prompt 환경에서 MSVC 탐지가 안정적이었다.
- `python tools/check_layers.py` 는 통과했지만, 기본 CP949 콘솔에서는 Unicode 출력 때문에 `UnicodeEncodeError` 가 날 수 있었다. `PYTHONIOENCODING=utf-8` 설정 후 안정적으로 실행됐다.
- 문서/스크립트 패치 중에도 콘솔 인코딩이 UTF-8 이 아니면 한글이 깨질 수 있었다. 운영 메모로 문서에 반영했다.

## 2026-03-15 검증 업데이트

- **빌드 환경**: 일반 bash에서 `cmake`를 직접 실행하면 MSVC를 찾지 못해 실패한다.
  반드시 VS Dev Shell(PowerShell)을 경유해야 한다:
  ```powershell
  powershell -NoProfile -Command "& 'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation; cmake --preset debug"
  ```
- **PowerShell 명령 구분**: `&&` 미지원, `;` 사용.
- `cmake --preset debug`, `cmake --build --preset debug`, `ctest --preset debug` 모두 VS Dev Shell 경유로 재검증 성공.
