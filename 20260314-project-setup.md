# 프로젝트 초기 설정

## 목표
빈 레포에서 빌드 가능한 CMake 스캐폴딩과 레이어 검사 툴을 갖춘 초기 상태 구성.

## 범위
- CMakeLists.txt (루트 + 서브디렉토리)
- src/ 디렉토리 구조 + 플레이스홀더
- tests/ 기본 구성 (GoogleTest)
- tools/check_layers.py 구현
- tools/requirements.txt
- .clang-format
- .github/workflows/ci.yml (빌드 + 테스트 + 레이어 검사)

## 완료 기준
- [ ] `cmake -B build && cmake --build build` 성공
- [ ] `cd build && ctest` 성공 (빈 테스트 스위트도 통과)
- [ ] `python3 tools/check_layers.py` 실행 성공 (위반 0건)
- [ ] GitHub Actions CI 파이프라인 녹색
- [ ] 모든 레이어 디렉토리 존재 (빈 파일이라도)

## 진행 상황
- [ ] CMakeLists.txt 작성
- [ ] 레이어 디렉토리 구조 생성
- [ ] check_layers.py 구현
- [ ] GoogleTest 연동
- [ ] CI 워크플로우 작성

## 설계 결정 기록
- 빌드 시스템: CMake 선택 (크로스 플랫폼, 업계 표준)
- 패키지 관리: FetchContent로 GoogleTest, nlohmann/json 가져옴 (vcpkg는 추후 도입 검토)
- CI: GitHub Actions (무료 플랜)
