# Exec Plan: 아이템 데이터 테이블 + 인스턴스 팩토리

> 생성: 2026-04-05
> 영역: `client/`

## 목적

서버 패킷으로부터 아이템 인스턴스를 생성하는 파이프라인을 구축한다.

- UE DataTable row 구조체 (`FClientItemTableRow`) 정의
- GameInstanceSubsystem (`UClientItemSubsystem`) 에서 DataTable 보유 + 팩토리 제공
- 패킷 수신 시 `ItemTid + 런타임 데이터 → UClientItemInstance` 생성 경로 완성

## 배경

`UClientItemInstance::InitializeFromData`가 이미 있지만,
`ItemTid → FClientItemStaticData` 조회 계층이 없어 패킷에서 바로 인스턴스를 만들 수 없다.

## 최종 아키텍처 결정 (2026-04-05 확정)

```
[item_table.schema.json 수정]  ← enum 정의 포함, 단일 소스
        ↓ python tools/generate_tables.py --schema ... --data-src ... --subdir item
        
client/Source/client/Public/Generated/ItemTableRow.h   ← UENUM + USTRUCT + UPROPERTY
client/Content/Data/item_table.data.json               ← 런타임 로드용 데이터
shared/generated/server/item/item_table.h              ← 서버 plain C++ (enum class, struct)

        ↓ UBT 빌드
UHT → ItemTableRow.generated.h 자동 생성

        ↓ 게임 시작
UClientItemSubsystem::Initialize() → JSON 자동 로드 → TMap<int32, FItemTableRow>
```

- `FClientItemStaticData` 및 `FClientItemTableRow` 제거 (생성된 `FItemTableRow`가 대체)
- `FItemTableRow` 자체를 USTRUCT(BlueprintType) + UPROPERTY로 Blueprint 직접 노출
- `item_data_table.md` → Schema JSON이 단일 소스이므로 deprecated 처리

## 단계

- [x] 1. 임시 `FClientItemTableRow` / `UClientItemSubsystem` 작성 (폐기 예정)
- [x] 2. `docs/proposal/ingame/data/item_table.schema.json` 작성
         - enum 블록 포함 (EItemCategory, EItemRarity, EEquipSlot, EWeaponType)
         - 기획서 item.md 기준 필드 전체
- [x] 3. `docs/proposal/ingame/data/item_table.data.json` 작성 (샘플 5개+)
- [x] 4. `tools/generate_tables.py` 작성
         - 단일 `.schema.json` 기준 범용 생성기
         - `--schema` 또는 `--schema-dir`(정확히 1개 schema 포함) 지원
         - 언리얼 출력: `client/Source/client/Public/Generated/ItemTableRow.h`
           * UENUM(BlueprintType) — 언리얼 enum 규칙 (E 접두사, uint8)
           * USTRUCT(BlueprintType) + GENERATED_BODY() + UPROPERTY
           * `#include "ItemTableRow.generated.h"` 마지막 include 유지
         - 데이터 출력: `client/Content/Data/item_table.data.json` (복사)
         - 서버 출력: `shared/generated/server/item/item_table.h`
           * plain enum class + plain struct (std::string, std::vector)
           * JSON 로더 함수 포함
         - 서버 매니페스트 출력: `shared/generated/server/item_data_tables.h`
- [x] 5. 생성 실행 및 파일 확인
- [x] 6. `UClientItemSubsystem` 교체
         - `Initialize()` 에서 JSON 자동 로드
         - `FClientItemStaticData` 의존 제거, `FItemTableRow` 직접 사용
         - `CreateItemInstance` 업데이트
- [x] 7. `FClientItemStaticData`, `FClientItemTableRow` 제거
- [x] 8. `docs/proposal/common/item_data_table.md` deprecated 처리
- [ ] 9. 빌드 + Blueprint/C++ 테스트
         - [x] 9a. generated.h include 위치 버그 수정 (구조체 정의 이전으로 이동)
         - [x] 9b. 헤더 재생성
         - [x] 9c. UClientItemInstance — Blueprint getter 추가 (GetStaticData, GetRuntimeData, GetCount, GetItemId)
         - [ ] 9d. 사용자 빌드 (UE Editor에서 직접)
         - [ ] 9e. Blueprint 테스트: 서브시스템 → CreateItemInstance → 결과 확인
         - [ ] 9f. C++ 테스트 (로그 출력으로 확인)

## 완료 기준

- [ ] schema 변경 → generator 실행 → rebuild 만으로 언리얼 코드 반영된다.
- [ ] `FItemTableRow`가 USTRUCT(BlueprintType)이고 모든 필드에 UPROPERTY가 있다.
- [ ] `UClientItemSubsystem::Initialize()`에서 JSON 자동 로드된다.
- [ ] 서버 헤더는 UE 의존 없이 plain C++이다.
- [ ] `item_data_table.md`가 deprecated 표시되거나 제거된다.
- [ ] Blueprint에서 CreateItemInstance 호출 후 유효한 인스턴스 반환됨.
- [ ] C++에서 서버 패킷 시뮬레이션 (ItemId + FClientItemRuntimeData) 동작.
