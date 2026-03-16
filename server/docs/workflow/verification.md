# Verification
> 빌드, 테스트, 린트, 인코딩 검사의 상세 명령과 환경 메모.
> `agent-workflow.md`에서는 이 문서를 참조만 한다.

## 사전 준비

- 작업 디렉터리는 `server/` 기준이다.
- `debug` preset은 `CMakePresets.json`에서 `Ninja` generator와 `VCPKG_ROOT` 기반 toolchain file을 전제로 한다.
- Windows에서는 최소한 다음 도구가 준비돼 있어야 한다.
  - CMake
  - Ninja
  - MSVC C++ 컴파일러(`cl.exe`)
  - Git
  - vcpkg
- Windows 환경 문제와 실제 우회 사례는 [build-troubleshooting.md](./build-troubleshooting.md)를 따른다.

## 사전 확인

### 기본 확인 명령

```powershell
cmake --version
ninja --version
git --version
$env:VCPKG_ROOT
where.exe cl
```

- `cmake --version`이 실패하면 CMake가 PATH에 없거나 설치되지 않은 상태다.
- `ninja --version`이 실패하면 Ninja가 PATH에 없거나 설치되지 않은 상태다.
- `git --version`이 실패하면 vcpkg manifest 해석 중 registry/baseline 조회가 실패할 수 있다.
- `$env:VCPKG_ROOT`가 비어 있으면 preset의 toolchain file 경로가 깨진다.
- `where.exe cl`이 비어 있으면 MSVC Developer Shell이 아니거나 C++ 빌드 도구가 설치되지 않은 상태다.

### 권장 경로 확인 예시

```powershell
Get-ChildItem 'C:\Program Files' -Recurse -Filter cmake.exe -ErrorAction SilentlyContinue | Select-Object -First 5 -ExpandProperty FullName
Get-ChildItem 'C:\Program Files' -Recurse -Filter ninja.exe -ErrorAction SilentlyContinue | Select-Object -First 5 -ExpandProperty FullName
Get-ChildItem 'C:\Program Files' -Recurse -Filter git.exe -ErrorAction SilentlyContinue | Select-Object -First 5 -ExpandProperty FullName
Get-ChildItem 'C:\Program Files\Microsoft Visual Studio' -Recurse -Filter cl.exe -ErrorAction SilentlyContinue | Select-Object -First 5 -ExpandProperty FullName
```

- 로컬 PATH가 비어 있어도 설치 위치를 먼저 찾은 뒤 절대 경로로 실행할 수 있다.
- 예시 경로
  - Visual Studio CMake: `C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`
  - Visual Studio Ninja: `C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe`
  - Git for Windows: `C:\Program Files\Git\cmd\git.exe`
  - Developer Shell: `C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat`

## 설치 가이드

- CMake / Ninja가 없으면 다음 중 하나를 준비한다.
  - Visual Studio Installer에서 C++ 개발 도구와 CMake/Ninja 구성요소 설치
  - CLion 번들 CMake/Ninja를 절대 경로로 사용
- `cl.exe`가 없으면 Visual Studio Installer에서 C++ 컴파일 도구를 포함한 워크로드를 설치한다.
  - 일반적으로 `Desktop development with C++` 또는 동등한 MSVC 빌드 도구 구성이 필요하다.
- Git이 없으면 Git for Windows를 설치하고 `git.exe` 경로가 확인되는지 검증한다.
- `VCPKG_ROOT`가 없으면 설치된 vcpkg 루트를 환경 변수로 지정한다.
  - Visual Studio 번들 경로 예시: `C:\Program Files\Microsoft Visual Studio\18\Community\VC\vcpkg`

## 기본 검증 순서

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
python tools/check_layers.py
python tools/check_encoding.py
```

## Windows / vcpkg 메모

- `VCPKG_ROOT`는 vcpkg 루트를 가리켜야 한다.
- `cmake --preset debug`는 Visual Studio Developer Shell에서 실행하는 편이 안전하다.
- 일반 PowerShell에서 `cl.exe`를 찾지 못하면 configure 또는 build가 실패할 수 있다.
- 일반 PowerShell에서 `Launch-VsDevShell.ps1`가 실행 정책으로 막히면 `VsDevCmd.bat`를 `cmd.exe /c`로 호출하는 편이 안전하다.
- 콘솔 인코딩 문제로 Python 출력이 깨지면 `PYTHONIOENCODING=utf-8`을 사용한다.
- 샌드박스 환경에서는 사용자 프로필, 전역 vcpkg cache, 네트워크 접근이 막힐 수 있으므로 필요 시 샌드박스 외부에서 실행한다.

### PowerShell 예시

```powershell
$vsdev = 'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1'

powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; cmake --preset debug"
powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; cmake --build --preset debug"
powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; ctest --preset debug"
```

### `VsDevCmd.bat` 예시

```powershell
cmd.exe /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && cmake --preset debug'
cmd.exe /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && cmake --build --preset debug'
cmd.exe /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && ctest --preset debug'
```

### PATH가 없을 때 절대 경로로 실행하는 예시

```powershell
cmd.exe /v:on /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && set "VCPKG_ROOT=C:\Program Files\Microsoft Visual Studio\18\Community\VC\vcpkg" && set "PATH=C:\Program Files\Git\cmd;!PATH!" && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -G Ninja -DCMAKE_MAKE_PROGRAM="C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" --preset debug'
```

## 추가 메모

- `vcpkg.json` manifest 모드를 쓰면 baseline 또는 configuration이 필요하다.
- 이 저장소는 `vcpkg.json`의 `builtin-baseline`과 외부 registry 조회가 가능해야 configure가 완료된다.
- 저장소 텍스트 파일은 UTF-8 without BOM으로 유지한다.
- 포맷 적용이 필요하면 변경 범위에 맞춰 `clang-format`을 사용한다.
