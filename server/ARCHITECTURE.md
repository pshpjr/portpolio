# ARCHITECTURE.md
> 프로젝트의 레이어 불변식과 각 레이어 책임을 정의한다.
> 구현 상태, 디렉터리 트리, 외부 라이브러리 목록은 참조 문서로 분리한다.

---

## 레이어 의존성 규칙

```
Types -> Config -> Core -> Service -> Network -> Runtime
```

- 각 레이어는 자신보다 왼쪽에 있는 레이어만 참조할 수 있다.
- 오른쪽 방향 의존성은 금지하며 공용 `../tools/check_layers.py`와 CI에서 검사한다.
- 같은 레이어 내부의 순환 의존성도 허용하지 않는다.

### 금지 예시

```cpp
// Core는 Network나 Service를 직접 참조하면 안 된다.
#include "network/packet.h"  // invalid
#include "service/party.h"   // invalid

// Core는 Types, Config만 참조할 수 있다.
#include "config/skill_data.h" // valid
#include "types/stat.h"        // valid
```

---

## 레이어 책임

### `Types` (`src/01_types/`)

- 공용 타입, enum, POD 구조체
- 외부 의존성 없이 가볍게 유지

### `Config` (`src/02_config/`)

- 데이터 정의 로더와 설정 구조
- `Types`만 의존

### `Core` (`src/03_core/`)

- 순수 게임 로직
- 네트워크, 세션, 런타임 개념을 몰라야 함
- 테스트 가능한 단위로 유지

### `Service` (`src/04_service/`)

- 여러 Core 기능을 조합한 게임플레이 흐름
- 순수 규칙보다 조립과 상태 전이에 집중

### `Network` (`src/05_network/`)

- 패킷 직렬화, 세션, AOI
- 도메인 규칙의 단일 소스가 아니라 전달과 동기화 계층

### `Runtime` (`src/06_runtime/`)

- 서버 엔트리포인트와 조립
- 설정, 실행 수명주기, 핫리로드 같은 운영 책임 담당

---

## 레이어별 핵심 판단 기준

- `Types`: 규칙이 아니라 데이터 표현만 둔다.
- `Config`: 파일 포맷 파싱과 로딩만 담당한다.
- `Core`: 입출력 없이도 테스트 가능해야 한다.
- `Service`: Core를 조합하지만 프로토콜 세부사항은 모른다.
- `Network`: 패킷/세션 처리와 게임 규칙 계산을 분리한다.
- `Runtime`: 각 계층을 연결하지만 비즈니스 규칙을 새로 만들지 않는다.

---

## 관련 참조

- 구현 상태: [docs/quality/grades.md](./docs/quality/grades.md)
- 외부 라이브러리: [docs/architecture/dependencies.md](./docs/architecture/dependencies.md)
- 구조 개요: [docs/reference/project-layout.md](./docs/reference/project-layout.md)
