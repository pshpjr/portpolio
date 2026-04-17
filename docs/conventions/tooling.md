# Python 툴링 컨벤션 (tooling.md)

## 역할

Python은 저장소에서 다음 용도로 사용한다.

- 공용 검사 도구
- 코드 생성
- 문서 유효성 검사
- 빌드/CI 보조 스크립트
- 개발용 유틸리티

게임 규칙의 단일 구현은 Python에 두지 않는다.

---

## 배치 원칙

- 저장소 공용 검사 도구는 루트 `tools/` 아래에 둔다.
- 앱 전용 스크립트는 각 앱의 `tools/` 아래에 둔다.
- 공용 규칙을 검사하는 스크립트를 특정 앱 하위에 두지 않는다.

예시:

```text
tools/
├── check_encoding.py
├── check_layers.py
├── context_meter.py
├── doc_check.py
├── git_commit.py
├── record_agent_artifact.py
└── setup_linux_env.sh

server/tools/
└── codegen/
```

---

## 버전 및 환경

```bash
Python 3.10+
```

- Windows 콘솔 인코딩 문제로 출력이 깨지면 `PYTHONIOENCODING=utf-8`을 사용한다.
- 도구는 단독 실행 가능하고 종료 코드가 명확해야 한다.

---

## 네이밍

| 대상 | 규칙 | 예시 |
|------|------|------|
| 파일/모듈 | snake_case | `check_layers.py`, `doc_check.py` |
| 클래스 | PascalCase | `LayerChecker`, `DocValidator` |
| 함수/변수 | snake_case | `check_include()`, `layer_map` |
| 상수 | UPPER_SNAKE_CASE | `LAYER_ORDER`, `LINK_PATTERN` |

---

## 공용 검사 도구

### `check_layers.py`

- 디렉터리 최상위 계층 이름을 기준으로 include 의존 방향을 검사한다.
- 서버와 `Lib/`처럼 서로 다른 구조도 `--root`, `--layer-order` 옵션으로 검사할 수 있어야 한다.

### `doc_check.py`

- 사람이 관리하는 마크다운 문서의 내부 링크를 검사한다.
- 각 영역의 작업 문서/노트 형식도 함께 확인한다.

### `check_encoding.py`

- 저장소 텍스트 파일이 UTF-8 without BOM인지 검사한다.
- 깨진 문자나 전형적인 mojibake 패턴을 감지한다.

### `context_meter.py`

- 작업에 실제로 읽은 컨텍스트 파일들과 현재 diff를 기준으로 대략적인 토큰 부하를 계산한다.
- 작업 분량 대비 컨텍스트/토큰 사용량을 비교해, 스크립트화·스킬화 후보를 찾는 데 사용한다.

### `git_commit.py`

- Bash 툴이 Windows 환경에서 `/mnt/` 경로를 인식하지 못하는 문제를 우회하는 git 래퍼.
- 저장소 루트를 자동 감지하므로 어느 경로에서 실행해도 동작한다.
- `Co-Authored-By` 서명을 자동으로 삽입한다.
- **에이전트는 git commit/add/status/diff가 필요할 때 이 스크립트를 사용한다.**

```bash
# 상태 확인
python tools/git_commit.py --status

# 특정 파일 스테이징 + 커밋
python tools/git_commit.py --add path/to/file.md -m "커밋 메시지"

# 변경된 추적 파일 전체 스테이징 + 커밋
python tools/git_commit.py --add-all -m "커밋 메시지"

# staged diff 확인
python tools/git_commit.py --diff

# 최근 커밋 5개 확인
python tools/git_commit.py --log
```

### `record_agent_artifact.py`

- 외부 LLM/OpenCode 응답 직후 구조화된 응답 노트를 `_workspace/agent-notes/`에 남긴다.
- 필요 시 `task` / `message` 후속 작업을 `harness-improvement-queue.md`에 자동 추가한다.

---

## 새 툴 추가 시 규칙

1. 공용인지 앱 전용인지 먼저 결정한다.
2. 공용이면 루트 `tools/`, 앱 전용이면 해당 앱 `tools/`에 둔다.
3. `--help` 또는 동등한 사용법을 제공한다.
4. 성공은 `0`, 실패는 `1` 이상으로 끝낸다.
5. 관련 `AGENTS.md`와 workflow 문서에서 호출 경로를 함께 갱신한다.
