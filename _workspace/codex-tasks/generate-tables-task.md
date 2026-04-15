# 작업: 범용 데이터 테이블 코드 생성기 + 아이템 스키마

## 참고 파일 먼저 읽기

1. `server/tools/codegen/combat_data_gen.py` (전체) — 기존 생성기 구조 파악
2. `shared/generated/unreal/combat/weapon_table.h` — 언리얼 출력 예시
3. `shared/generated/server/combat/weapon_table.h` — 서버 출력 예시
4. `docs/proposal/ingame/data/weapon_table.schema.json` — 스키마 형식 참고
5. `docs/proposal/ingame/data/weapon_table.data.json` — 데이터 형식 참고

---

## 생성할 파일 1: docs/proposal/ingame/data/item_table.schema.json

```json
{
  "table": "ItemTable",
  "keys": [
    { "field": "ItemId", "unique": true }
  ],
  "fields": [
    { "name": "ItemId",                        "type": "int32" },
    { "name": "ItemCode",                      "type": "string" },
    { "name": "ItemName",                      "type": "string" },
    { "name": "ItemCategory",                  "type": "enum", "enum_name": "EItemCategory" },
    { "name": "Rarity",                        "type": "enum", "enum_name": "EItemRarity" },
    { "name": "EquipSlot",                     "type": "enum", "enum_name": "EEquipSlot" },
    { "name": "WeaponType",                    "type": "enum", "enum_name": "EWeaponType" },
    { "name": "MaxStackCount",                 "type": "int32" },
    { "name": "MaxDurability",                 "type": "int32" },
    { "name": "MaxTradeCount",                 "type": "int32" },
    { "name": "SellValue",                     "type": "int32" },
    { "name": "bCanUseSharedStorage",          "type": "bool" },
    { "name": "bCanBindToConsumableQuickSlot", "type": "bool" },
    { "name": "IconKey",                       "type": "string" },
    { "name": "BalanceVersion",                "type": "int32" }
  ],
  "enums": [
    { "name": "EItemCategory", "values": ["NONE","WEAPON","ARMOR","CONSUMABLE","SKILL_BOOK","MATERIAL","QUEST","CURRENCY"] },
    { "name": "EItemRarity",   "values": ["NORMAL","UNCOMMON","RARE","EPIC"] },
    { "name": "EEquipSlot",    "values": ["NONE","WEAPON","CHEST","LEGS","HANDS","NECKLACE","RING_LEFT","RING_RIGHT"] },
    { "name": "EWeaponType",   "values": ["NONE","SWORD_SHIELD","GREATSWORD","STAFF"] }
  ]
}
```

---

## 생성할 파일 2: docs/proposal/ingame/data/item_table.data.json

5개 샘플 아이템 (JSON 배열):

```json
[
  {
    "ItemId": 4001, "ItemCode": "sword_shield_w1", "ItemName": "검방 T1",
    "ItemCategory": "WEAPON", "Rarity": "NORMAL", "EquipSlot": "WEAPON", "WeaponType": "SWORD_SHIELD",
    "MaxStackCount": 1, "MaxDurability": 120, "MaxTradeCount": 3, "SellValue": 0,
    "bCanUseSharedStorage": true, "bCanBindToConsumableQuickSlot": false,
    "IconKey": "weapon_sword_shield", "BalanceVersion": 1
  },
  {
    "ItemId": 4002, "ItemCode": "greatsword_w1", "ItemName": "대검 T1",
    "ItemCategory": "WEAPON", "Rarity": "NORMAL", "EquipSlot": "WEAPON", "WeaponType": "GREATSWORD",
    "MaxStackCount": 1, "MaxDurability": 110, "MaxTradeCount": 3, "SellValue": 0,
    "bCanUseSharedStorage": true, "bCanBindToConsumableQuickSlot": false,
    "IconKey": "weapon_greatsword", "BalanceVersion": 1
  },
  {
    "ItemId": 4101, "ItemCode": "armor_chest_t1", "ItemName": "전사 상의",
    "ItemCategory": "ARMOR", "Rarity": "NORMAL", "EquipSlot": "CHEST", "WeaponType": "NONE",
    "MaxStackCount": 1, "MaxDurability": 80, "MaxTradeCount": 3, "SellValue": 0,
    "bCanUseSharedStorage": true, "bCanBindToConsumableQuickSlot": false,
    "IconKey": "armor_chest_t1", "BalanceVersion": 1
  },
  {
    "ItemId": 4102, "ItemCode": "armor_legs_t1", "ItemName": "전사 하의",
    "ItemCategory": "ARMOR", "Rarity": "NORMAL", "EquipSlot": "LEGS", "WeaponType": "NONE",
    "MaxStackCount": 1, "MaxDurability": 70, "MaxTradeCount": 3, "SellValue": 0,
    "bCanUseSharedStorage": true, "bCanBindToConsumableQuickSlot": false,
    "IconKey": "armor_legs_t1", "BalanceVersion": 1
  },
  {
    "ItemId": 4201, "ItemCode": "potion_hp_small", "ItemName": "회복 영약 소",
    "ItemCategory": "CONSUMABLE", "Rarity": "NORMAL", "EquipSlot": "NONE", "WeaponType": "NONE",
    "MaxStackCount": 5, "MaxDurability": 0, "MaxTradeCount": 0, "SellValue": 50,
    "bCanUseSharedStorage": false, "bCanBindToConsumableQuickSlot": true,
    "IconKey": "potion_hp_small", "BalanceVersion": 1
  }
]
```

---

## 생성할 파일 3: tools/generate_tables.py (범용 생성기)

### 설계 원칙
- 특정 테이블에 종속되지 않음. 스키마를 읽어 완전 자동 생성.
- `--schema-dir` 에서 지정한 특정 `.schema.json` 파일 하나만 처리.
  (또는 단일 파일 경로 `--schema` 옵션 지원)
- `enums` 블록 → 언리얼용 UENUM / 서버용 enum class 자동 생성
- `keys` 배열 → unique=true 단일맵, unique=false 멀티맵 인덱스 빌더 생성

### argparse 인터페이스

```
python tools/generate_tables.py \
  --schema docs/proposal/ingame/data/item_table.schema.json \
  --server-out shared/generated/server \
  --unreal-out client/Source/client/Public/Generated \
  --data-src docs/proposal/ingame/data/item_table.data.json \
  --data-out client/Content/Data \
  --subdir item
```

기본값 (argparse default):
- `--server-out` : `<REPO_ROOT>/shared/generated/server`
- `--unreal-out` : `<REPO_ROOT>/client/Source/client/Public/Generated`
- `--data-out`   : `<REPO_ROOT>/client/Content/Data`
- `--subdir`     : `""` (빈 문자열 = 서브디렉토리 없음)

REPO_ROOT는 `Path(__file__).resolve().parents[1]` 로 계산.

### keys 처리 규칙
- `unique: true` → `TMap<KeyType, FRow>` / `std::unordered_map<KeyType, Row>`
- `unique: false` → `TMap<KeyType, TArray<FRow>>` / `std::unordered_map<KeyType, std::vector<Row>>`
- keys 없으면 인덱스 빌더 생성 안 함 (rows 배열만 반환하는 로더만 생성)

### enums 처리 규칙
- 스키마의 `enums` 배열에서 enum 이름과 값 목록 읽기
- 같은 이름의 enum이 중복 정의된 경우: 값이 동일하면 허용, 다르면 ValueError 발생

---

## 언리얼 출력: `client/Source/client/Public/Generated/{TableName}Row.h`

파일 구조 (반드시 이 순서):

```cpp
// Auto-generated. Do not edit.
#pragma once
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// ============================================================
// Enums (스키마 enums 블록에서 생성)
// ============================================================
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    NONE     UMETA(DisplayName = "NONE"),
    WEAPON   UMETA(DisplayName = "WEAPON"),
    // ...
};

// 각 enum마다 파싱 헬퍼
inline bool TryParseEItemCategory(const FString& Value, EItemCategory& Out)
{
    // 각 값을 if/else로 비교
}

// ... 다른 enum들 동일하게 ...

// ============================================================
// Row Struct
// ============================================================
USTRUCT(BlueprintType)
struct F{TableName}Row
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 ItemId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString ItemCode{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    EItemCategory ItemCategory = EItemCategory::NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    bool bCanUseSharedStorage = false;

    // ... 모든 필드 ...
};

// ============================================================
// JSON Loaders (inline)
// ============================================================
inline bool Load{TableName}Row(const TSharedPtr<FJsonObject>& Object,
                                F{TableName}Row& OutRow, FString& OutError)
{
    // bool: TryGetBoolField
    // int32: TryGetNumberField -> static_cast<int32>
    // FString: TryGetStringField
    // enum: TryGetStringField -> TryParseE...
}

inline bool Load{TableName}(const FString& JsonText,
                              TArray<F{TableName}Row>& OutRows, FString& OutError)
{
    // root가 배열 또는 {"rows": [...]} / {"data": [...]} 형태 모두 지원
}

inline bool Load{TableName}FromFile(const FString& FilePath,
                                     TArray<F{TableName}Row>& OutRows, FString& OutError)
{
    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *FilePath)) { ... }
    return Load{TableName}(JsonText, OutRows, OutError);
}

// ============================================================
// Index Builders (keys 배열 기반으로 생성)
// ============================================================

// unique=true 인 키 ItemId → TMap<int32, FItemTableRow>
inline TMap<int32, F{TableName}Row> Build{TableName}IndexByItemId(
    const TArray<F{TableName}Row>& Rows)
{
    TMap<int32, F{TableName}Row> Index;
    for (const auto& Row : Rows) Index.Add(Row.ItemId, Row);
    return Index;
}

// unique=false 인 키 예시 (멀티맵)
inline TMap<int32, TArray<F{TableName}Row>> Build{TableName}IndexByRewardId(
    const TArray<F{TableName}Row>& Rows)
{
    TMap<int32, TArray<F{TableName}Row>> Index;
    for (const auto& Row : Rows) Index.FindOrAdd(Row.RewardId).Add(Row);
    return Index;
}

// ============================================================
// 반드시 파일 마지막 줄 (UHT 필수)
// ============================================================
#include "{TableName}Row.generated.h"
```

**중요: `#include "{TableName}Row.generated.h"` 는 절대 마지막 줄.**

---

## 서버 출력: `shared/generated/server/{subdir}/{snake_table_name}.h`

파일 구조:

```cpp
// Auto-generated. Do not edit.
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>

// JSON 파싱: combat 서버 헤더와 완전히 동일한 방식
// (nlohmann/json 또는 직접 파서 — combat_data_gen.py 서버 출력 참고)
// UE 헤더 절대 사용 금지

// ============================================================
// Enums
// ============================================================
enum class EItemCategory : uint8_t
{
    NONE = 0,
    WEAPON,
    // ...
};

inline bool TryParseEItemCategory(const std::string& value, EItemCategory& out)
{
    // if/else 비교
}

// ============================================================
// Row Struct
// ============================================================
struct ItemTableRow
{
    int32_t ItemId{};
    std::string ItemCode{};
    EItemCategory ItemCategory{};
    bool bCanUseSharedStorage{};
    // ...
};

// ============================================================
// JSON Loaders
// ============================================================
inline bool LoadItemTableRow(/* 서버 JSON 오브젝트 타입 */, ItemTableRow& out, std::string& error) { ... }
inline bool LoadItemTable(const std::string& json_text, std::vector<ItemTableRow>& out, std::string& error) { ... }
inline bool LoadItemTableFromFile(const std::string& file_path, std::vector<ItemTableRow>& out, std::string& error) { ... }

// ============================================================
// Index Builders
// ============================================================
// unique=true → std::unordered_map<int32_t, ItemTableRow>
// unique=false → std::unordered_map<int32_t, std::vector<ItemTableRow>>
```

서버 JSON 파싱 방식은 `combat_data_gen.py`가 생성한 서버 헤더와 동일하게 구현.

---

## 매니페스트 출력: `shared/generated/server/item_data_tables.h`

`shared/generated/server/combat_data_tables.h` 패턴 참고:
- `kItemDataTables` constexpr 배열 (FTableDescriptor 목록)
- `ItemDataBundle` struct (각 테이블 rows TArray/vector 포함)
- `LoadItemDataBundleFromDirectory` 함수

---

## 데이터 파일 복사: `client/Content/Data/item_table.data.json`

`item_table.data.json` 내용을 이 경로에 그대로 복사 출력.

---

## 실행 및 검증

파일 작성 완료 후 반드시 실행:

```bash
python tools/generate_tables.py \
  --schema docs/proposal/ingame/data/item_table.schema.json \
  --data-src docs/proposal/ingame/data/item_table.data.json \
  --subdir item
```

다음 파일이 존재하는지 확인:
1. `client/Source/client/Public/Generated/ItemTableRow.h`
2. `client/Content/Data/item_table.data.json`
3. `shared/generated/server/item/item_table.h`
4. `shared/generated/server/item_data_tables.h`

에러 발생 시 수정 후 재실행.

---

## 주의사항

- `combat_data_gen.py` 수정 금지
- `generate_tables.py`는 item에 종속되지 않는 완전 범용 생성기
  (다른 스키마를 넣어도 동일하게 동작)
- Python 3.10+ 타입 힌트 스타일
- 서버 헤더에 UE 심볼(`CoreMinimal.h`, `FString`, `TArray`, `UPROPERTY` 등) 절대 금지
- 언리얼 헤더 로더는 `FFileHelper`, `FJsonSerializer` 사용
