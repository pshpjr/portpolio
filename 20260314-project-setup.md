# 프로젝트 초기 설정

## 목표
빈 레포에서 빌드 가능한 CMake 스캐폴딩과 레이어 검사 툴을 갖춘 초기 상태 구성.

## 범위
- CMakeLists.txt (루트 + 서브디렉토리)
- vcpkg.json (manifest 모드 의존성 선언)
- CMakePresets.json (debug / release / ci 프리셋)
- src/ 디렉토리 구조 + 플레이스홀더
- tests/ 기본 구성 (GoogleTest)
- tools/check_layers.py 구현
- tools/requirements.txt
- .clang-format
- .github/workflows/ci.yml (빌드 + 테스트 + 레이어 검사)

## 완료 기준
- [ ] `cmake --preset debug` 성공 (vcpkg 패키지 설치 포함)
- [ ] `cmake --build --preset debug` 성공
- [ ] `ctest --preset debug` 성공 (빈 테스트 스위트도 통과)
- [ ] `python3 tools/check_layers.py` 실행 성공 (위반 0건)
- [ ] GitHub Actions CI 파이프라인 녹색
- [ ] 모든 레이어 디렉토리 존재 (빈 파일이라도)

## 진행 상황
- [x] vcpkg.json 작성 (spdlog, flatbuffers, asio, nlohmann-json, gtest)
- [x] CMakePresets.json 작성 (debug / release / ci)
- [ ] CMakeLists.txt 작성
- [ ] 레이어 디렉토리 구조 생성
- [ ] check_layers.py 구현
- [ ] GoogleTest 연동
- [ ] CI 워크플로우 작성

## 설계 결정 기록
- 빌드 시스템: CMake 선택 (크로스 플랫폼, 업계 표준)
- 패키지 관리: vcpkg manifest 모드 채택 (`vcpkg.json` + `CMakePresets.json`)
  - `VCPKG_ROOT` 환경 변수로 툴체인 경로 주입 → 로컬/CI 모두 동일한 방식
  - 향후 서드파티 라이브러리 추가 시 `vcpkg.json`의 `dependencies` 배열에 항목만 추가
  - FetchContent 미사용: 버전 고정 및 재현성 확보를 위해 vcpkg로 일원화
- CI: GitHub Actions (무료 플랜)
