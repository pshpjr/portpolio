# src 레이어 폴더 번호 접두사 추가

## 목표
`src/` 하위 레이어 폴더에 레이어 순서를 나타내는 번호 접두사를 붙여 탐색기/IDE에서 의존성 순서대로 정렬되도록 한다.

## 변경 매핑

| 기존 | 변경 후 |
|------|---------|
| `src/types/` | `src/01_types/` |
| `src/config/` | `src/02_config/` |
| `src/core/` | `src/03_core/` |
| `src/service/` | `src/04_service/` |
| `src/network/` | `src/05_network/` |
| `src/runtime/` | `src/06_runtime/` |

## 범위
- `src/` 하위 6개 폴더 rename (git mv)
- 모든 소스 파일 내 `#include "layer/..."` → `#include "NN_layer/..."` 수정
- `tools/check_layers.py` — `NN_` 접두사 자동 제거 로직 추가
- `ARCHITECTURE.md` — 경로 레퍼런스 갱신
- `AGENTS.md` — 도메인 목록 경로 갱신

## 완료 기준
- [ ] 6개 폴더 rename 완료
- [ ] 모든 소스 파일 include 경로 갱신
- [x] `python tools/check_layers.py` 통과 (위반 0건)
- [x] `cmake --preset debug && cmake --build --preset debug` 성공
- [x] `ctest --preset debug` 성공
- [x] ARCHITECTURE.md, AGENTS.md 경로 갱신

## 진행 상황
- [x] 6개 폴더 rename (git mv)
- [x] `tools/check_layers.py` 접두사 제거 로직 추가 — 검사 통과 확인
- [x] `ARCHITECTURE.md` 경로 갱신
- [x] `AGENTS.md` 경로 갱신
- [x] 소스 파일 내 #include 경로 갱신

## 설계 결정
- `check_layers.py`의 `LAYER_ORDER`는 `["types", ...]` 그대로 유지, `_strip_prefix()` 헬퍼로 `NN_` 제거 후 매칭
  - 이유: include 경로(`#include "01_types/..."`)와 폴더명 모두 처리 가능하며, LAYER_ORDER 가독성 유지