#pragma once

#include "CoreMinimal.h"
#include "Generated/game_data/game_data_tables.h"
#include "ClientInventoryTypes.generated.h"

class UClientItemInstance;

// EItemCategory, EItemRarity, EEquipSlot, EWeaponType 은 Generated/ItemTableRow.h 에서 옵니다.

UENUM(BlueprintType)
enum class EClientItemStorageKind : uint8
{
    None,
    Inventory,
    Equipment,
    SharedStorage,
    ConsumableQuickSlot,
    SkillHotSlot,
    Loot
};

UENUM(BlueprintType)
enum class EClientSkillType : uint8
{
    None,
    Active,
    Identity,
    Mobility,
    Parry
};

UENUM(BlueprintType)
enum class EClientConsumableSlot : uint8
{
    None,
    Slot1,
    Slot2
};

// 기획: 소모품 슬롯 최대 수량 (슬롯당 5개)
inline constexpr int32 GClientConsumableSlotMaxStack = 5;

// 기획: 인벤토리/창고 슬롯 확장 상한 및 기본 오픈 수량.
// 서버가 보내 주는 현재 용량(Current)이 Max보다 작으면 초과 슬롯은 잠금 상태로 UI에 표시한다.
inline constexpr int32 GClientInventoryMaxCapacity = 48;
inline constexpr int32 GClientInventoryDefaultCapacity = 24;
inline constexpr int32 GClientStorageTabMaxCapacity = 48;
inline constexpr int32 GClientStorageTabDefaultCapacity = 24;

UENUM(BlueprintType)
enum class EClientSkillHotSlot : uint8
{
    None,
    Active1,
    Active2,
    Active3,
    Active4,
    Identity
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientItemLocation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EClientItemStorageKind StorageKind = EClientItemStorageKind::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 SlotIndex = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 SecondaryIndex = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EEquipSlot EquipSlot = EEquipSlot::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EClientSkillHotSlot SkillHotSlot = EClientSkillHotSlot::None;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientItemRuntimeData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FString ItemKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Count = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Durability = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 RemainingTradeCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    bool bIsAccountBound = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FClientItemLocation Location;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientStorageTabDescriptor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
    FName TabId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
    int32 Capacity = 0;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientStorageTabState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
    FName TabId = NAME_None;

    /** 현재 열려 있는 슬롯 수. `Slots`는 Max 크기로 할당되고 Capacity 초과 인덱스는 잠금 상태. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
    int32 Capacity = 0;

    /** 이 탭의 확장 상한. 기본은 `GClientStorageTabMaxCapacity`. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
    int32 MaxCapacity = GClientStorageTabMaxCapacity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage")
    TArray<TObjectPtr<UClientItemInstance>> Slots;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientSkillDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString SkillKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    int32 SkillTid = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    EClientSkillType SkillType = EClientSkillType::Active;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    EWeaponType RequiredWeaponType = EWeaponType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FText DisplayName;
};
