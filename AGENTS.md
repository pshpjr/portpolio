# AGENTS.md
> 이 파일은 AI 에이전트를 위한 진입점(목차)입니다.
> 세부 내용은 반드시 아래 링크된 문서를 읽으십시오.

---

## 프로젝트 한 줄 요약
1~4인 인스턴스 던전 RPG의 **게임 서버** 포트폴리오 프로젝트.
---

## 에이전트가 작업을 시작할 때 해야 할 것

1. `AGENTS.md` 읽기
2. `docs/exec-plans/active/` 확인 — 진행 중인 작업이 있는가?
3. 관련 도메인 문서 읽기 (`ARCHITECTURE.md`, 해당 컨벤션 파일)
4. 작업 범위 파악 후 실행 계획 파일 작성 (없는 경우)
5. 레이어 규칙 확인 후 코딩 시작
6. 빌드 및 테스트 통과 확인
7. `tools/check_layers.py` 실행
8. PR 설명에 변경 이유와 영향 범위 명시

## 문서 구조
| 문서 | 목적 |
|------|------|
| [ARCHITECTURE.md](./ARCHITECTURE.md) | 도메인 전체 지도 + 레이어 의존성 규칙 |
| [docs/design/overview.md](./docs/design/overview.md) | 프로젝트 목표, 범위, 비범위 |
| [docs/design/core-beliefs.md](./docs/design/core-beliefs.md) | 설계 철학 및 에이전트 운영 원칙 |
| [docs/conventions/cpp-style.md](./docs/conventions/cpp-style.md) | C++ 코딩 컨벤션 (필수) |
| [docs/conventions/tooling.md](./docs/conventions/tooling.md) | Python 툴링 컨벤션 |
| [docs/workflow/agent-workflow.md](./docs/workflow/agent-workflow.md) | 에이전트 작업 흐름 (PR 생성까지) |

---

## 빠른 커맨드 레퍼런스

> **Windows 필수**: 모든 빌드/테스트 명령은 VS Dev Shell을 통해 실행해야 MSVC가 탐지된다.
> 일반 bash/PowerShell에서 `cmake`를 직접 실행하면 컴파일러를 찾지 못해 실패한다.

```powershell
# VS Dev Shell 경유 실행 방법 (PowerShell)
# - 명령 구분은 `;` 사용 (PowerShell에서 `&&` 미지원)
$vsdev = 'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1'

# Configure
powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; cmake --preset debug"

# 빌드
powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; cmake --build --preset debug"

# 테스트
powershell -NoProfile -Command "& '$vsdev' -Arch amd64 -SkipAutomaticLocation; ctest --preset debug"
```

```bash
# 린트 (아키텍처 레이어 위반 검사)
# 인코딩 문제 시 PYTHONIOENCODING=utf-8 설정
PYTHONIOENCODING=utf-8 python tools/check_layers.py

# 인코딩 검사 (텍스트 파일은 UTF-8 without BOM)
python tools/check_encoding.py

# 문서 유효성 검사
python tools/doc_check.py
```

---

## 레이어 의존성 (요약 - 위반 금지)

```
Types → Config → Core → Service → Network → Runtime
```
각 레이어는 왼쪽 방향으로만 의존 가능. 상세: [docs/architecture/layers.md](./docs/architecture/layers.md)

---

## 도메인 목록

- `src/01_types/` — 공용 타입, enum, 구조체
- `src/02_config/` — 데이터 드리븐 설정 로더 (JSON/FlatBuffers)
- `src/03_core/` — 전투, 스탯, 아이템 등 게임 로직
- `src/04_service/` — 파티, 매칭, 인스턴스 관리
- `src/05_network/` — 패킷 핸들러, 세션, AOI
- `src/06_runtime/` — 서버 엔트리포인트, 핫리로드
- `tools/` — Python 유틸리티 (코드 생성, 검사, 문서화)
- `data/` — 게임 데이터 정의 파일
- `tests/` — 단위/통합 테스트

---

## 에이전트 행동 원칙 (요약)

1. **레이어 위반 절대 금지** — 빌드 전 `tools/check_layers.py` 실행
2. **작업 전 문서 먼저** — 관련 도메인 문서를 읽은 후 코드 작성
3. **exec-plan 먼저 작성** — 작업 시작 전 `docs/exec-plans/active/`에 계획 파일 생성
4. **모든 설계 결정은 `docs/design/`에 기록**
5. **테스트 없는 PR은 열지 않음**

상세: [docs/design/core-beliefs.md](./docs/design/core-beliefs.md)
