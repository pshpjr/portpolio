# Exec Plan: Client Inventory Foundation

## 목표

- 언리얼 클라이언트에서 사용할 아이템 인스턴스 클래스 초안을 만든다.
- 게임 기획의 인벤토리, 장비, 공유 창고, 스킬 핫슬롯 규칙을 담는 컴포넌트 골격을 추가한다.
- 서버 프로토콜의 `item_key`, `item_tid`, `skill_key`, `skill_tid`, 위치 정보 개념을 클라이언트 도메인 모델에 반영한다.

## 범위

- 클라이언트 코드:
  - `client/Source/client/Public/Inventory/`
  - `client/Source/client/Private/Inventory/`
  - 필요 시 `client/Source/client/client.Build.cs`
- 루트 문서:
  - `docs/exec-plans/active/INDEX.md`

## 완료 기준

- [x] 아이템 정적/동적 상태를 담는 언리얼용 타입과 베이스 아이템 클래스가 생긴다.
- [x] 인벤토리 컴포넌트가 슬롯 초기화, 적재, 이동, 제거, 스택 병합의 기본 API를 제공한다.
- [x] 장비 컴포넌트가 장착 슬롯 검증과 장착/해제 API를 제공한다.
- [x] 창고 컴포넌트가 공유 창고 탭/슬롯 모델과 입출고 API를 제공한다.
- [x] 스킬 핫슬롯 컴포넌트가 `Q/E/R/F` 중심의 등록/해제/조회 API를 제공한다.
- [x] 주요 타입과 메서드가 블루프린트 연동 가능한 형태로 노출된다.
- [x] 최소 1회 이상 빌드 또는 정적 점검 시도를 하고 결과를 기록한다.

## 진행 상황

- [x] 관련 루트/기획 문서 및 프로토콜 조사
- [x] exec-plan 작성 및 범위 확정
- [x] 클라이언트 인벤토리 도메인 타입 설계
- [x] 아이템/컴포넌트 코드 추가
- [x] 빌드 영향 점검

## 설계 결정 기록

- `client/` 전용 AGENTS/exec-plan 체계가 아직 없으므로, 이번 구현 계획은 루트 `docs/exec-plans/active/`에서 관리한다.
- 아이템은 월드 액터보다 인벤토리/장비/UI 상태 공유가 쉬운 `UObject` 기반 인스턴스 클래스로 시작한다.
- 인벤토리, 장비, 창고, 스킬 핫슬롯은 각각 별도 `UActorComponent`로 분리해 UI와 플레이어 폰/컨트롤러가 필요한 조합만 붙일 수 있게 한다.
- 기획 문서의 장비/공유 창고/소모품 슬롯/무기 기반 스킬 구조를 반영하되, 서버 확정 패킷 세부 규칙은 후속 동기화 레이어에서 연결한다.
- `Build.bat clientEditor Win64 Development ...` 실행은 시도했지만, 현재 프로젝트 전역의 Marketplace 플러그인 두 개가 동일한 `ItemDataAsset.h` 파일명을 가져 UHT가 먼저 실패했다. 이번 초안 코드의 직접 컴파일 결과는 이 충돌 해소 후 다시 확인해야 한다.
