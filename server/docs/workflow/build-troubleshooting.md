# Build Troubleshooting
> Windows 기준 서버 configure/build/test 장애 대응 기록.
> 반복되는 환경 문제와 실제 우회 방법을 이 문서에 누적한다.

## 이번 검증에서 실제로 해결한 환경 문제

### 1. `cmake`가 PATH에 없음

- 증상: `cmake --preset debug` 시작 자체가 실패한다.
- 확인:

```powershell
cmake --version
```

- 해결:
  - Visual Studio 또는 CLion에 포함된 `cmake.exe` 절대 경로를 사용한다.
  - 예시: `C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`

### 2. `VCPKG_ROOT`가 비어 있음

- 증상: preset의 toolchain file 경로를 해석하지 못한다.
- 확인:

```powershell
$env:VCPKG_ROOT
```

- 해결:
  - 세션에서 `VCPKG_ROOT`를 직접 지정한다.
  - 예시: `C:\Program Files\Microsoft Visual Studio\18\Community\VC\vcpkg`

### 3. `Launch-VsDevShell.ps1`가 실행 정책에 막힘

- 증상: PowerShell에서 `PSSecurityException`이 발생한다.
- 해결:
  - `Launch-VsDevShell.ps1` 대신 `VsDevCmd.bat`를 `cmd.exe /c`로 호출한다.

```powershell
cmd.exe /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && cmake --preset debug'
```

### 4. `vcpkg` cache 쓰기 위치 제한

- 증상: `C:\Users\<user>\AppData\Local\vcpkg\downloads` 생성 또는 쓰기 단계에서 실패한다.
- 해결:
  - 필요하면 `LOCALAPPDATA` 또는 `VCPKG_DOWNLOADS`를 쓰기 가능한 작업 디렉터리 하위로 바꾼다.

```powershell
$env:LOCALAPPDATA = "$PWD\\.cache\\localappdata"
$env:VCPKG_DOWNLOADS = "$PWD\\.cache\\localappdata\\vcpkg\\downloads"
```

### 5. `vcpkg` 전역 락 대기

- 증상: `waiting to take filesystem lock on ...\.vcpkg-root` 메시지가 반복된다.
- 해결:
  - 다른 IDE/터미널에서 돌아가는 vcpkg 작업이 없는지 확인한다.
  - 필요하면 샌드박스 외부 실행으로 전환한다.

### 6. GitHub / 프록시 접근 실패

- 증상: `WinHttpSendRequest failed with exit code 12029`
- 영향: `vcpkg.json`의 `builtin-baseline`과 registry 정보를 읽지 못해 configure가 끝나지 않는다.
- 해결:
  - 인터넷이 되는 환경에서 다시 실행한다.
  - 사내 프록시 환경이면 `HTTP_PROXY`, `HTTPS_PROXY`, Windows 프록시 설정을 점검한다.
  - 사내 vcpkg 미러 또는 binary cache가 있으면 그 설정을 사용한다.
- 확인:

```powershell
git --version
Test-NetConnection github.com -Port 443
```

### 7. `cl.exe`를 CMake가 자동 발견하지 못함

- 증상: `No CMAKE_CXX_COMPILER could be found`
- 확인:

```powershell
cmd.exe /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && where cl'
```

- 해결:
  - `VsDevCmd.bat` 안에서는 `where cl`이 되는데 CMake가 캐시를 못 채우면 `-DCMAKE_CXX_COMPILER=`로 명시한다.
  - 예시: `C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64\cl.exe`

### 8. Windows SDK 도구가 링크 단계에서 안 보임

- 증상: `RC Pass 1: command "rc ... failed"` 또는 `CMAKE_MT-NOTFOUND`
- 원인:
  - Developer Shell이 잡아 준 PATH를 후속 `set PATH=...`로 덮어써서 `rc.exe`, `mt.exe` 경로가 사라질 수 있다.
- 확인:

```powershell
cmd.exe /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && where rc && where mt'
```

- 해결:
  - `PATH`를 완전히 덮어쓰지 말고 기존 Developer Shell PATH 앞에 필요한 경로만 prepend 한다.
  - `cmd.exe /v:on`과 `!PATH!`를 사용하면 `VsDevCmd.bat` 이후의 PATH를 유지할 수 있다.

```powershell
cmd.exe /v:on /c '"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 && set "PATH=C:\Program Files\Git\cmd;!PATH!" && cmake --preset debug'
```

## 이번 검증에서 빌드가 실제로 진행된 지점

- `vcpkg` manifest 의존성 설치 완료
- CMake configure 완료
- Ninja build 시작 확인
- 최종 실패는 환경이 아니라 코드 컴파일 단계에서 발생

현재 확인된 코드 실패:

- `05_network/executor/network_executor.h` 헤더를 찾지 못해 `packet_handler.*`, `session_manager.*` 빌드 실패

이 문제는 환경 트러블슈팅 범위를 넘어가므로 코드 수정으로 해결해야 한다.

## 사용자에게 요청해야 하는 경우

- Visual Studio C++ 빌드 도구가 없다면 설치를 요청한다.
- Git, CMake, Ninja가 전혀 없으면 설치를 요청한다.
- 네트워크가 막혀 있거나 프록시가 필요한 환경이면 다음 정보를 요청한다.
  - GitHub 접근 가능 여부
  - 사내 프록시 주소 또는 VPN 필요 여부
  - 사내 vcpkg 미러 / binary cache 존재 여부
- 샌드박스에서 전역 경로, 사용자 프로필 경로, 네트워크 접근이 필요한 경우 샌드박스 외부 실행 승인을 요청한다.
