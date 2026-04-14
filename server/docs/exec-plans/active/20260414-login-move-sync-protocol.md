# Exec Plan: 로그인 → 이동 / 유저 정보 동기화 프로토콜 정의

## Goal

로그인·이동·유저 정보 동기화에 해당하는 **프로토콜 메시지(.proto)만** 정의한다.
핸들러, 세션 상태머신, 네트워크 코드, DB 연동은 이번 plan 범위 밖이다.

시나리오:

1. 클라이언트가 닉네임을 실어 `RqLogin`을 보낸다. (로그인/계정 개념은 없다 — 닉네임 = 임시 식별자)
2. 서버가 `RsLogin`으로 내 `UserInfo`, 시작 `TransformInfo`, 필요한 초기 스냅샷을 바로 내려준다.
3. 클라이언트는 `RqMove`로 입력/목표 지점을 보낸다.
4. 서버는 내 최신 위치·상태를 `BrUserSnapshot` (브로드캐스트 이벤트)로 주기 전송한다.

---

## Scope

### 포함

- `shared/protocol/*.proto` 메시지 추가/정리
- `shared/protocol/build-proto.py`가 생성하는 헤더 경로가 그대로 동작하도록 패키지/파일 규칙 유지
- `shared/generated/` 산출물 재생성 (빌드 툴 실행)

### 제외

- 서버측 핸들러 등록, `packet_handler.h` 맵 갱신
- `Connection` 상태머신 개편
- 클라이언트측 수신/파싱 코드
- DB/세션/인증 로직
- 좌표 압축·보간·주기 튜닝

---

## 현재 상태 요약

- `shared/protocol/xxx.Common.proto` — `InstanceKey`만 정의 (package 없음, 최상위)
- `shared/protocol/Game.struct.proto` — package `C2G`. `UserInfo`, `StatInfo(빈)`, `CurrencyInfo`, `TransformInfo`, `IntPointInfo`, `EnumStorageType`, `ItemInfo` 정의
- `shared/protocol/Game.packet.proto` — package `C2G`, 현재 비어 있음
- `build-proto.py` — 파일 prefix(`Game`, `xxx`)별로 헤더를 생성. `Rq`/`Rs` 로 시작하는 message만 `PACKET_TYPE` enum에 등록

### 기존 구조의 쟁점 (사용자 확정 필요)

struct.proto 가 `C2G` 패키지로 묶여 있어, 서버→클라 메시지(`G2C`)에서 공유하기 어렵다.
본 plan에서는 다음 두 옵션 중 하나를 선택해야 한다.

- **옵션 A (권장)** — struct 들을 중립 패키지로 옮긴다.
  - `xxx.Common.proto` 에 기존 구조체들을 흡수 (또는 `Game.struct.proto` 의 package 를 제거/변경)
  - `C2G`, `G2C` 양쪽 packet 이 import 해서 공유
  - 추가 작업: 기존 struct.proto 의 package 선언 변경, 이를 참조하는 곳은 아직 없으므로 회귀 위험 낮음
- **옵션 B** — struct 을 두 벌로 유지
  - 빠르지만 동기화 비용이 즉시 생긴다. 기각 추천

→ 기본안 **A**로 진행. 사용자 반대 시 B 또는 변형으로 교체.

---

## 프로토콜 설계 (초안)

### 패키지 분리

| 파일 | 패키지 | 방향 | 비고 |
|---|---|---|---|
| `xxx.Common.proto` | (top-level) | 공용 | `InstanceKey` 유지 |
| `Game.struct.proto` | `Common` (변경) | 공용 | 기존 구조체들을 중립 패키지로 이동 |
| `Game.c2g.packet.proto` (신규) | `C2G` | Client → Server | `Rq*` 요청 |
| `Game.g2c.packet.proto` (신규) | `G2C` | Server → Client | `Rs*` 응답, `Br*` 브로드캐스트 |

기존 `Game.packet.proto` 는 `Game.c2g.packet.proto` 로 개명하여 역할을 명시한다 (C2G 패키지만 들어 있으므로 의미가 같음).
> 대안: 그대로 두고 G2C 파일만 추가. build-proto.py 는 파일명 prefix가 같으면 같은 헤더로 aggregate 하므로, C2G/G2C 를 한 헤더(`Game.header.h`)로 묶고 싶다면 둘 다 `Game.*.proto` 로 두면 된다. 현재 그대로 유지 권장.

### struct.proto 보강

기존 항목 유지, 아래 항목만 추가/확정.

```proto
// 1회성 캐릭터 상태 스냅샷. RsLogin/BrUserSnapshot 양쪽에서 재사용.
message UserSnapshot
{
    UserInfo        user        = 1;
    TransformInfo   transform   = 2;
    StatInfo        stat        = 3;   // 내용은 추후 별도 plan
    // v1 에서는 인벤토리/통화 스냅샷은 별도 패킷 그룹에서 다룸
}
```

- `StatInfo` 는 비어 있는 상태 유지 (다른 plan 범위).
- 움직임 입력 방식은 "목표 지점 이동"으로 단순화한다. 방향 벡터, 속도 동기화는 별도 plan.

### C2G 메시지

```proto
syntax = "proto3";
package C2G;
import "xxx.Common.proto";
import "Game.struct.proto";

// 로그인 (임시: 닉네임만)
message RqLogin
{
    string nickname = 1;
}

// 이동 입력 — 목표 지점으로 이동
message RqMove
{
    IntPointInfo    target      = 1;
    int64           client_ts   = 2;   // 클라 송신 타임스탬프(ms). 서버 보정/검증용
}
```

### G2C 메시지

```proto
syntax = "proto3";
package G2C;
import "xxx.Common.proto";
import "Game.struct.proto";

// 로그인 응답 — 성공 시 내 캐릭터 스냅샷을 즉시 하달
enum EnumLoginResult
{
    LoginResultNone         = 0;
    LoginResultOk           = 1;
    LoginResultNicknameBusy = 2;
    LoginResultInvalid      = 3;
}

message RsLogin
{
    EnumLoginResult result  = 1;
    UserSnapshot    me      = 2;   // result == Ok 일 때만 의미 있음
}

// 이동 응답 — 서버가 확정한 목표/현재 좌표
message RsMove
{
    InstanceKey     user_key    = 1;
    TransformInfo   confirmed   = 2;   // 서버 권위로 채택된 위치
    int64           server_ts   = 3;
}

// 주기 브로드캐스트 — 내 정보(그리고 이후 파티원/타 플레이어) 상태 갱신
message BrUserSnapshot
{
    UserSnapshot    snapshot    = 1;
}
```

- `Br*` 는 build-proto.py 의 `Rq|Rs` 정규식에 걸리지 않는다. 브로드캐스트 이벤트를 `PACKET_TYPE` 에 포함시키려면 정규식을 `^(Rq|Rs|Br|Ev)` 로 확장해야 한다. 이번 plan 에서 정규식을 함께 손보는 것을 제안.
  - 대안: v1 에서는 `Rs` 로 통일 (`RsUserSnapshot` 처럼). 다만 `요청/응답/브로드캐스트 분리` 방침(proposal/server/protocol.md)에 맞지 않으므로 정규식 확장을 권장.

### 상태 전이와 결부된 암묵 규약 (문서화만)

- 서버는 `RqLogin` 이후 최초 1회 `RsLogin` 을 내린 뒤, 동일 세션에서 `BrUserSnapshot` 을 주기적으로 송신한다.
- 이번 plan 에서는 세션 상태머신(`eLogin` 등) 변경은 하지 않는다. `connection.h` 의 `EnumConnectionState` 는 그대로 둔다.
- 정확한 주기/송신 조건은 후속 plan 에서 확정한다.

---

## 작업 단계

1. 이 exec-plan 사용자 검토/승인
2. struct.proto package 를 `Common` 으로 변경 (옵션 A)
3. `Game.c2g.packet.proto` 에 `RqLogin`, `RqMove` 정의
4. `Game.g2c.packet.proto` 신규 작성 (`RsLogin`, `RsMove`, `BrUserSnapshot`, `EnumLoginResult`)
5. `build-proto.py` 정규식을 `^(Rq|Rs|Br|Ev)[A-Za-z0-9_]*$` 로 확장
6. `python shared/protocol/build-proto.py` 실행 → `shared/generated/server/`, `shared/generated/unreal/` 재생성
7. 생성 산출물 커밋 여부 결정 (현 저장소에 checked-in 되어 있는지 사용자 확인 필요)
8. 변경된 .proto 의 import 경로가 클라이언트 빌드/서버 빌드를 깨뜨리지 않는지 확인 (현재 핸들러 연결 없음 → 실질 영향은 build-proto.py 재생성까지)

---

## Completion Criteria

- `shared/protocol/*.proto` 에 위 메시지들이 정의되어 있고 protoc 컴파일 통과
- `shared/generated/` 산출물이 갱신되고 aggregated header 에 새 메시지들이 등록됨
- 서버/클라 기존 빌드가 본 변경만으로는 깨지지 않음 (핸들러 미등록으로 참조가 없음)
- exec-plan 상태 갱신 및 완료 시 `completed/` 이동

---

## Open Questions (사용자 확정 필요)

1. `Game.struct.proto` 의 package 를 `Common` 으로 바꾸는 것(옵션 A) 에 동의하는지.
2. 이동 입력 방식을 "목표 지점 기반(`RqMove.target`)" 으로 고정해도 되는지. (방향 기반이나 continuous input 는 별도 plan)
3. 브로드캐스트 메시지를 `Br*` 접두사로 두고 build-proto.py 정규식을 확장하는 방침에 동의하는지. (대안: `Rs*` 로 통일)
4. `shared/generated/` 산출물이 저장소에 checked-in 되는지 (있다면 재생성 후 커밋). 없으면 빌드 파이프라인이 자동 생성하는지.
5. 이번 plan 에서 `RsMove` 는 내 입력에 대한 개별 응답으로 둘지, 또는 `BrUserSnapshot` 만으로 통일할지. (권장: `BrUserSnapshot` 만 두고 `RsMove` 는 v2 로 미룬다.)

---

## Progress

- [pending] exec-plan 사용자 검토/승인
- [pending] struct.proto package 조정
- [pending] C2G 패킷 파일 추가 (`RqLogin`, `RqMove`)
- [pending] G2C 패킷 파일 추가 (`RsLogin`, `BrUserSnapshot`, 필요 시 `RsMove`)
- [pending] build-proto.py 정규식 확장
- [pending] 코드 생성 + 산출물 반영
