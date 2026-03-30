#pragma once

#include "CoreMinimal.h"
#include "ClientInventoryTypes.generated.h"

class UClientItemInstance;

UENUM(BlueprintType)
enum class EClientItemCategory : uint8
{
    None,
    Weapon,
    Armor,
    Consumable,
    SkillBook,
    Material,
    Quest,
    Currency
};

UENUM(BlueprintType)
enum class EClientWeaponType : uint8
{
    None,
    SwordAndShield,
    Greatsword,
    Staff
};

UENUM(BlueprintType)
enum class EClientItemRarity : uint8
{
    Normal,
    Uncommon,
    Rare,
    Epic
};

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
enum class EClientEquipSlot : uint8
{
    None,
    Weapon,
    Head,
    Chest,
    Hands,
    Legs,
    Feet,
    Necklace,
    RingLeft,
    RingRight
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
enum class EClientSkillHotSlot : uint8
{
    None,
    Q,
    E,
    R,
    F,
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
    EClientEquipSlot EquipSlot = EClientEquipSlot::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EClientSkillHotSlot SkillHotSlot = EClientSkillHotSlot::None;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientItemStaticData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 ItemTid = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EClientItemCategory Category = EClientItemCategory::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EClientWeaponType WeaponType = EClientWeaponType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EClientItemRarity Rarity = EClientItemRarity::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EClientEquipSlot EquipSlot = EClientEquipSlot::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxStackCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxDurability = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxTradeCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    bool bCanUseSharedStorage = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    bool bCanBindToConsumableQuickSlot = false;
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
struct CLIENT_API FClientInventorySlotState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UClientItemInstance> Item = nullptr;
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
    int32 Capacity = 0;

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
    EClientWeaponType RequiredWeaponType = EClientWeaponType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FText DisplayName;
};
