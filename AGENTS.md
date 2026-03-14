# AGENTS.md
> 이 파일은 AI 에이전트를 위한 진입점(목차)입니다. 백과사전이 아닙니다.
> 세부 내용은 반드시 아래 링크된 문서를 읽으십시오.

---

## 프로젝트 한 줄 요약
2~4인 인스턴스 던전 RPG의 **게임 서버** 포트폴리오 프로젝트.
서버사이드 히트 판정, 데이터 드리븐 설정, 런타임 핫리로드가 핵심 기술 목표.

---

## 필수 선행 독서 (작업 전 반드시 확인)

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

```bash
# 빌드
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j$(nproc)

# 테스트
cd build && ctest --output-on-failure

# 린트 (아키텍처 레이어 위반 검사)
python3 tools/check_layers.py

# 문서 유효성 검사
python3 tools/doc_check.py

# 코드 포맷
clang-format -i $(find src -name "*.cpp" -o -name "*.h")
```

---

## 레이어 의존성 (요약 - 위반 금지)

```
Types → Config → Core → Service → Network → Runtime
```
각 레이어는 왼쪽 방향으로만 의존 가능. 상세: [docs/architecture/layers.md](./docs/architecture/layers.md)

---

## 도메인 목록

- `src/types/` — 공용 타입, enum, 구조체
- `src/config/` — 데이터 드리븐 설정 로더 (JSON/FlatBuffers)
- `src/core/` — 전투, 스탯, 아이템 등 게임 로직
- `src/service/` — 파티, 매칭, 인스턴스 관리
- `src/network/` — 패킷 핸들러, 세션, AOI
- `src/runtime/` — 서버 엔트리포인트, 핫리로드
- `tools/` — Python 유틸리티 (코드 생성, 검사, 문서화)
- `data/` — 게임 데이터 정의 파일
- `tests/` — 단위/통합 테스트

---

## 현재 진행 중인 작업

[docs/exec-plans/active/](./docs/exec-plans/active/) 디렉토리를 확인하십시오.
작업 시작 전 반드시 active 플랜이 있는지 확인하고, 없으면 새 플랜 파일을 먼저 만드십시오.

---

## 에이전트 행동 원칙 (요약)

1. **레이어 위반 절대 금지** — 빌드 전 `tools/check_layers.py` 실행
2. **작업 전 문서 먼저** — 관련 도메인 문서를 읽은 후 코드 작성
3. **실패 시 "더 열심히"가 아니라 "무엇이 빠졌는가"** 질문
4. **모든 설계 결정은 `docs/design/`에 기록** — Slack/채팅이 아닌 레포가 진실
5. **테스트 없는 PR은 열지 않음**

상세: [docs/design/core-beliefs.md](./docs/design/core-beliefs.md)
