# 리눅스 빌드/실행 가능성 검증

## 목표
Linux 환경에서도 프로젝트 표준 빌드 체인(vcpkg + Ninja)으로 configure/build/test(실행)가 동작하도록 보완한다.

## 범위
- CMakePresets.json
- CMakeLists.txt
- src/01_types/entity_id.h
- tools/setup_linux_env.sh
- docs/workflow/agent-workflow.md
- docs/exec-plans/active/20260316-linux-build-validation.md

## 완료 기준
- [x] Linux 전용 preset이 vcpkg + Ninja를 명시
- [x] Linux 사전 준비 스크립트(없으면 설치/설정) 추가
- [x] Linux에서 configure/build/test 성공
- [x] tools/check_layers.py 성공
- [x] 검증/운영 메모를 문서에 기록

## 진행 상황
- [x] 사용자 피드백 반영 방향 확정 (Linux도 vcpkg + Ninja 강제)
- [x] preset/CMake 정리 (FetchContent fallback 제거, vcpkg 기반 복원)
- [x] Linux 환경 bootstrap 스크립트 추가
- [x] 빌드/테스트/레이어 검사 재검증

## 설계 결정 기록
- 의존성 일관성과 재현성을 위해 Linux에서도 `find_package(GTest CONFIG REQUIRED)` + vcpkg manifest 방식을 유지한다.
- `debug-linux` preset은 Linux에서만 노출되며, generator는 Ninja로 고정한다.
- `tools/setup_linux_env.sh`는 다음을 자동 수행한다:
  - ninja 미설치 시 apt 기반 설치 시도
  - vcpkg 미설치 시 `$HOME/.local/vcpkg`에 clone + bootstrap
  - `VCPKG_ROOT`를 `~/.bashrc`에 추가
- GCC 빌드 정합성을 위해 `EntityId` 헤더의 `<functional>` include는 유지한다.

## 2026-03-16 검증 결과
- `bash tools/setup_linux_env.sh` 실행 (ninja 기설치 확인, vcpkg 신규 설치 + bootstrap + VCPKG_ROOT 등록).
- `VCPKG_ROOT=/root/.local/vcpkg cmake --preset debug-linux` 성공.
- `VCPKG_ROOT=/root/.local/vcpkg cmake --build --preset debug-linux` 성공.
- `VCPKG_ROOT=/root/.local/vcpkg ctest --preset debug-linux` 성공 (1/1).
- `PYTHONIOENCODING=utf-8 python tools/check_layers.py` 통과.
