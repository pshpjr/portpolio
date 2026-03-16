# 에이전트 작업 흐름 (agent-workflow.md)

## 작업 시작 체크리스트

새 세션에서 작업을 시작할 때:

```
□ AGENTS.md 읽기
□ docs/exec-plans/active/INDEX.md 확인
  → 진행 중인 플랜이 있으면: 해당 파일을 읽고 컨텍스트 복원
  → 없으면: 새 플랜 파일 작성 후 시작
□ 관련 도메인 문서 읽기
□ ARCHITECTURE.md 레이어 규칙 재확인
□ 문서 작업이면 문서 검증 스크립트 존재 여부 확인
```

---

## 실행 계획 파일 (exec-plan) 형식

`docs/exec-plans/active/` 에 작성. 파일명: `YYYYMMDD-작업명.md`

```markdown
# [작업명]

## 목표
한 문장으로 이 작업이 달성해야 할 것.

## 범위
- 수정/생성할 파일 목록
- 영향받는 레이어

## 완료 기준
- [ ] 빌드 통과
- [ ] 테스트 통과
- [ ] check_layers.py 통과
- [ ] 구체적인 기능 기준 1
- [ ] 구체적인 기능 기준 2

## 진행 상황
- [x] 완료된 항목
- [ ] 남은 항목

## 설계 결정 기록
작업 중 내린 결정과 이유를 여기에 기록.
(나중에 docs/design/으로 이동 가능)
```

작업 완료 시 `docs/exec-plans/completed/`로 이동.

## Skill / Agent 저장 위치

- 저장소 공용 skill은 루트 `.codex/skills/` 와 `.claude/skills/` 아래에 생성한다.
- 저장소 공용 agent 프롬프트는 루트 `.codex/agents/` 와 `.claude/agents/` 아래에 생성한다.
- 앱 디렉터리 아래에 skill 저장소를 만들지 않는다.
- Codex UI용 `openai.yaml`은 `.codex/skills/<skill>/agents/openai.yaml`에만 둔다.

---

## PR 작성 기준

PR을 열기 전 반드시:

```bash
# 0. Windows 사전 조건
# - VCPKG_ROOT 가 vcpkg 루트를 가리켜야 함 (권장: C:\vcpkg)
# - Visual Studio Developer Command Prompt 또는 VsDevCmd.bat 로 MSVC 환경을 먼저 로드

# 1. 빌드 확인
cmake --preset debug
cmake --build --preset debug

# 2. 테스트 통과
ctest --preset debug

# 3. 레이어 위반 검사
python tools/check_layers.py

# 4. 인코딩 검사
python tools/check_encoding.py

# 5. 문서 작업이면 링크/plan 검사
python server/tools/doc_check.py

# 6. 포맷 적용
clang-format -i $(find src -name "*.cpp" -o -name "*.h")
```

### Linux / vcpkg / Ninja 메모

- Linux도 `vcpkg manifest + Ninja`를 기본 빌드 체인으로 사용한다.
- `VCPKG_ROOT` 가 비어 있으면 configure가 실패한다.
- `ninja` 가 없으면 configure 단계에서 `CMAKE_MAKE_PROGRAM` 오류가 발생한다.
- 초기 환경 구성은 아래 스크립트로 자동화한다.

```bash
bash tools/setup_linux_env.sh
export VCPKG_ROOT="$HOME/.local/vcpkg"
```

- Linux 검증 커맨드:

```bash
VCPKG_ROOT="$HOME/.local/vcpkg" cmake --preset debug-linux
VCPKG_ROOT="$HOME/.local/vcpkg" cmake --build --preset debug-linux
VCPKG_ROOT="$HOME/.local/vcpkg" ctest --preset debug-linux
```

### Windows / vcpkg 메모

- `vcpkg.json` manifest 모드를 사용할 때는 `builtin-baseline` 또는 `vcpkg-configuration.json` 이 반드시 있어야 한다.
- `CMakePresets.json` 의 `toolchainFile` 은 `$env{VCPKG_ROOT}` 를 사용하므로, `VCPKG_ROOT` 가 비어 있으면 configure 단계에서 즉시 실패한다.
- `cmake --preset debug` 는 일반 PowerShell보다 Visual Studio 개발자 셸에서 재현성이 높다. 일반 셸에서는 `cl.exe` 를 찾지 못해 실패할 수 있다.
- `VCPKG_ROOT` 는 `C:\vcpkg` 같이 공백 없는 경로가 preset/toolchain 해석에 안전하다.
- `check_layers.py` 는 콘솔 인코딩에 따라 Unicode 출력 오류가 날 수 있다. Windows 콘솔에서 문제가 나면 `PYTHONIOENCODING=utf-8` 로 실행한다.
- `python` 이 Microsoft Store stub 경로만 잡히면 Python 스크립트 실행이 실패할 수 있다. 이 경우 실제 Python 설치 경로를 PATH에 추가한 뒤 검증을 실행한다.
- 저장소 텍스트 파일은 UTF-8 without BOM 으로 유지한다. `python tools/check_encoding.py` 가 0으로 끝나지 않으면 인코딩 오류로 간주한다.

PR 설명 필수 포함:
- **무엇을 했나**: 변경 내용 요약
- **왜 했나**: 동기, 설계 결정
- **영향 범위**: 수정된 레이어/도메인
- **테스트**: 어떻게 검증했나

---

## 에이전트가 막혔을 때

1. "더 열심히 시도"하지 않는다
2. 무엇이 빠진지 분석:
   - 문서가 부족한가? → `docs/`에 추가
   - 컨벤션이 불명확한가? → `docs/conventions/`에 추가
   - 린터가 없어 실수를 반복하는가? → `tools/`에 검사 추가
3. 부족한 것을 채운 후 다시 시도

---

## 금지 행동

- 레이어 의존성 위반 코드 커밋
- 테스트 없는 Core 레이어 신규 코드
- `check_layers.py` 통과 전 PR 생성
- exec-plan 없이 대형 작업 시작 (300줄 이상 변경 예상 시)
- 문서를 코드보다 나중에 작성 (설계 결정은 코딩 전 또는 동시에)
- 문서 검증 스크립트가 있는데도 문서 변경 후 확인하지 않기
