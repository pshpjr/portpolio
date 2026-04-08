# Exec Plan: 네트워크 라이브러리 구현

> 생성: 2026-04-08  
> 영역: `server/src/network/`, `server/lib/`, `docs/proposal/client/`

## 목표

서버 TCP 네트워크 레이어와 클라이언트 `UClientNetworkSubsystem`을 구현 가능한 상태로 만든다.  
구현 착수 전 설계 미확정 항목을 논의 문서(`docs/proposal/network-impl-decisions.md`)에서 결정하고, 결정 내용을 `protocol.md` 및 관련 기획서에 반영한다.

## 선행 조건 (구현 착수 전 완료 필요)

아래 항목이 미결인 상태에서는 서버/클라이언트 네트워크 코드를 작성하지 않는다.  
논의 문서: [`docs/proposal/network-impl-decisions.md`](../../proposal/network-impl-decisions.md)

| 항목 | 블로커 여부 | 논의 문서 섹션 |
|---|---|---|
| 패킷 프레이밍 포맷 (헤더 구조·엔디안) | 필수 | §1.1 |
| 패킷 타입 ID 할당 규칙 | 필수 | §1.2 |
| 서버 I/O 모델 (비동기 방식·스레드 구조) | 필수 | §2.1 |
| 서버 세션 관리 (최대 연결·타임아웃) | 필수 | §2.2 |
| EntityId 고유성 범위 (pending-decisions #17) | 필수 | §3.1 |
| 서버 공통 에러 응답 포맷 (pending-decisions #18) | 필수 | §1.3 |
| EntityType enum 도입 여부 (pending-decisions #14) | 필수 | §3.2 |
| 구역 전환 패킷 프로토콜 (pending-decisions #15) | 필수 | §3.3 |
| Tick 소유권 확정 (pending-decisions #13) | 클라이언트 구현 착수 전 | §4.1 |
| 재연결 grace period 정책 (pending-decisions #16) | 선택적 (v2 가능) | §3.4 |
| Tick당 Pop 처리 상한 (pending-decisions #13) | 선택적 | §4.2 |

## 범위

### 서버 (`server/src/network/`)

- TCP 리스닝 소켓 + accept 루프
- 세션(연결) 생성·폐기·상태 관리
- 수신 버퍼 + 패킷 프레이밍 (length-prefix)
- 송신 큐 + 비동기 send
- 패킷 타입 → 핸들러 라우팅
- 연결 끊김 감지 및 정리

### 공용 (`server/lib/` 검토 후 결정)

- 프레이밍·직렬화 보조 코드가 서버와 클라이언트에서 공유 가능하면 `server/lib/`에 배치
- 엔진 의존성이 생기는 순간 각 앱으로 분리

### 클라이언트 (`client/` — 언리얼 프로젝트)

- `UClientNetworkSubsystem` C++ 구현  
  (기획 계약: `docs/proposal/client/network_subsystem.md`)
- `UClientWorldEntitySubsystem` C++ 구현
- `FClientRemoteCharacterState`, `AClientRemoteCharacter` 기본 뼈대

## 단계

- [ ] **0. 선행 결정**: `network-impl-decisions.md`에서 필수 항목 전체 확정
- [ ] **1. protocol.md 갱신**: 프레이밍 포맷·패킷 타입 ID·에러 응답 포맷 추가
- [ ] **2. 서버 네트워크 레이어 구현**
  - [ ] 2-1. 소켓 수신·accept 루프
  - [ ] 2-2. 세션 클래스 (현재 `Connection` 확장)
  - [ ] 2-3. 패킷 프레이밍 (수신 버퍼 + 헤더 파싱)
  - [ ] 2-4. 패킷 핸들러 라우팅 (현재 `HandlerMap` 확장)
  - [ ] 2-5. 송신 큐 + 비동기 send
- [ ] **3. 클라이언트 네트워크 서브시스템 구현**
  - [ ] 3-1. `UClientNetworkSubsystem` (Connect/Disconnect/Send/Pop, 수신·송신 스레드)
  - [ ] 3-2. `UClientWorldEntitySubsystem` (Spawn/Move/Despawn 핸들러, State Map)
  - [ ] 3-3. `AClientRemoteCharacter` 뼈대 (Mesh, 보간 대상 Transform)
- [ ] **4. 연동 검증**
  - [ ] 4-1. 더미 서버 또는 실 서버와 핸드셰이크 테스트
  - [ ] 4-2. 패킷 왕복 (echo 또는 로그인 흐름) 확인

## 완료 기준

- [ ] 서버가 클라이언트의 연결을 수락하고 패킷을 수신·역직렬화·라우팅한다.
- [ ] 클라이언트가 서버에 연결하고 패킷을 송수신한다.
- [ ] `OnPacketReceived` Delegate가 게임 스레드 Tick에서 발생한다.
- [ ] 연결 끊김·재연결 시나리오에서 크래시 없이 상태가 정리된다.
- [ ] `server/docs/exec-plans/active/INDEX.md`와 `docs/exec-plans/active/INDEX.md`가 이 plan을 참조한다.

## 범위 외

- 클라이언트 예측 / 서버 Reconcile (v2)
- TLS / 암호화
- 재연결 자동화 (v2)
- 보간 알고리즘 고도화 (구현 단계에서 선형 보간으로 시작)
