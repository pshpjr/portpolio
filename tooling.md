# Python 툴링 컨벤션 (tooling.md)

## 역할
Python은 이 프로젝트에서 **게임 로직 코드를 작성하지 않는다.**
Python의 역할:
- 코드 생성 (FlatBuffers 스키마 → C++ 래퍼 등)
- 정적 분석 / 린팅 (레이어 의존성 검사)
- 문서 유효성 검사
- 빌드/CI 자동화 스크립트
- 테스트 클라이언트 (간단한 시뮬레이션)
- 기획 및 개발을 위한 각종 유틸리티 프로그램
---

## 버전 및 환경

```bash
Python 3.10+
가상환경: python3 -m venv .venv && source .venv/bin/activate
의존성: pip install -r tools/requirements.txt
```

---

## 네이밍

| 대상 | 규칙 | 예시 |
|------|------|------|
| 파일/모듈 | snake_case | `check_layers.py`, `doc_check.py` |
| 클래스 | PascalCase | `LayerChecker`, `DocValidator` |
| 함수/변수 | snake_case | `check_include()`, `layer_map` |
| 상수 | UPPER_SNAKE_CASE | `LAYER_ORDER`, `SRC_ROOT` |

---

## 툴 파일 구조

```
tools/
├── requirements.txt
├── check_layers.py       # 레이어 의존성 검사 (CI 필수)
├── doc_check.py          # 문서 유효성 검사
└── codegen/
    ├── __init__.py
    ├── flatbuffers_gen.py  # FlatBuffers → C++ 래퍼 생성
    └── packet_gen.py       # 패킷 핸들러 보일러플레이트 생성
```

---

## check_layers.py 동작 방식

`src/` 디렉토리의 모든 `.h`/`.cpp` 파일에서 `#include` 를 파싱하여
레이어 위반 여부를 검사한다.

```python
LAYER_ORDER = ["types", "config", "core", "service", "network", "runtime"]

# 각 파일의 레이어를 경로에서 추출
# src/core/combat/combat_system.h → layer = "core"
# include "network/packet.h"      → dep_layer = "network"
# core < network → 위반!
```

종료 코드:
- `0`: 위반 없음
- `1`: 위반 발견 (CI 실패)

---

## 새 툴 추가 시 규칙

1. `tools/` 디렉토리에 위치
2. 단독 실행 가능해야 함 (`python3 tools/my_tool.py`)
3. `--help` 옵션 지원
4. 종료 코드 명확히: 성공=0, 실패=1
5. `AGENTS.md`의 빠른 커맨드 레퍼런스에 추가

---

## 코드 스타일

```python
# type hint 필수
def check_include(file_path: str, include: str) -> bool:
    ...

# docstring (공개 함수)
def get_layer(path: str) -> str:
    """파일 경로에서 레이어 이름을 추출한다.
    
    Args:
        path: src/ 기준 상대 경로 (e.g. "core/combat/combat_system.h")
    
    Returns:
        레이어 이름 (e.g. "core")
    
    Raises:
        ValueError: 알 수 없는 레이어 경로인 경우
    """
    ...
```
