#!/usr/bin/env bash
set -euo pipefail

if [[ "$(uname -s)" != "Linux" ]]; then
  echo "[setup_linux_env] Linux 전용 스크립트입니다."
  exit 1
fi

install_ninja_if_missing() {
  if command -v ninja >/dev/null 2>&1; then
    echo "[setup_linux_env] ninja 이미 설치됨: $(command -v ninja)"
    return
  fi

  echo "[setup_linux_env] ninja가 없어 설치를 시도합니다."
  if command -v apt-get >/dev/null 2>&1; then
    if [[ "$(id -u)" -eq 0 ]]; then
      apt-get update
      apt-get install -y ninja-build
    elif command -v sudo >/dev/null 2>&1; then
      sudo apt-get update
      sudo apt-get install -y ninja-build
    else
      echo "[setup_linux_env] sudo/루트 권한이 없어 ninja 자동 설치를 할 수 없습니다."
      echo "[setup_linux_env] 수동 설치: apt-get install -y ninja-build"
      exit 1
    fi
  else
    echo "[setup_linux_env] apt-get 미지원 환경입니다. ninja를 수동 설치하세요."
    exit 1
  fi

  echo "[setup_linux_env] ninja 설치 완료: $(command -v ninja)"
}

setup_vcpkg_root() {
  local candidate="${VCPKG_ROOT:-$HOME/.local/vcpkg}"

  if [[ ! -d "$candidate" ]]; then
    echo "[setup_linux_env] vcpkg가 없어 $candidate 에 설치합니다."
    mkdir -p "$(dirname "$candidate")"
    git clone https://github.com/microsoft/vcpkg.git "$candidate"
  fi

  if [[ ! -x "$candidate/vcpkg" ]]; then
    echo "[setup_linux_env] vcpkg bootstrap 실행"
    "$candidate/bootstrap-vcpkg.sh" -disableMetrics
  fi

  export VCPKG_ROOT="$candidate"
  echo "[setup_linux_env] VCPKG_ROOT=$VCPKG_ROOT"

  if ! grep -q 'export VCPKG_ROOT=' "$HOME/.bashrc" 2>/dev/null; then
    echo "export VCPKG_ROOT=$candidate" >> "$HOME/.bashrc"
    echo "[setup_linux_env] ~/.bashrc 에 VCPKG_ROOT 추가 완료"
  else
    echo "[setup_linux_env] ~/.bashrc 에 VCPKG_ROOT 항목이 이미 존재합니다."
  fi

  if ! grep -q 'export VCPKG_ROOT=' "$HOME/.profile" 2>/dev/null; then
    echo "export VCPKG_ROOT=$candidate" >> "$HOME/.profile"
    echo "[setup_linux_env] ~/.profile 에 VCPKG_ROOT 추가 완료"
  else
    echo "[setup_linux_env] ~/.profile 에 VCPKG_ROOT 항목이 이미 존재합니다."
  fi
}

install_ninja_if_missing
setup_vcpkg_root

echo "[setup_linux_env] 완료. 현재 셸에서 사용하려면: export VCPKG_ROOT=$HOME/.local/vcpkg"
