# Verification
> 빌드, 테스트, 린트, 인코딩 검사의 상세 명령과 환경 메모.
> `agent-workflow.md`에서는 이 문서를 참조만 한다.

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
- 콘솔 인코딩 문제로 Python 출력이 깨지면 `PYTHONIOENCODING=utf-8`을 사용한다.

### PowerShell 예시

```powershell
$vsdev = 'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1'

powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; cmake --preset debug"
powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; cmake --build --preset debug"
powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; ctest --preset debug"
```

## 추가 메모

- `vcpkg.json` manifest 모드를 쓰면 baseline 또는 configuration이 필요하다.
- 저장소 텍스트 파일은 UTF-8 without BOM으로 유지한다.
- 포맷 적용이 필요하면 변경 범위에 맞춰 `clang-format`을 사용한다.
