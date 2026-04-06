// Auto-generated. Do not edit.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "ItemTableRow.generated.h"

// ============================================================
// Enums
// ============================================================
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    None UMETA(DisplayName = "None"),
    Weapon UMETA(DisplayName = "Weapon"),
    Armor UMETA(DisplayName = "Armor"),
    Consumable UMETA(DisplayName = "Consumable"),
    SkillBook UMETA(DisplayName = "SkillBook"),
    Material UMETA(DisplayName = "Material"),
    Quest UMETA(DisplayName = "Quest"),
    Currency UMETA(DisplayName = "Currency")
};

inline bool TryParseEItemCategory(const FString& Value, EItemCategory& Out)
{
    if (Value == TEXT("None"))
    {
        Out = EItemCategory::None;
        return true;
    }
    else if (Value == TEXT("Weapon"))
    {
        Out = EItemCategory::Weapon;
        return true;
    }
    else if (Value == TEXT("Armor"))
    {
        Out = EItemCategory::Armor;
        return true;
    }
    else if (Value == TEXT("Consumable"))
    {
        Out = EItemCategory::Consumable;
        return true;
    }
    else if (Value == TEXT("SkillBook"))
    {
        Out = EItemCategory::SkillBook;
        return true;
    }
    else if (Value == TEXT("Material"))
    {
        Out = EItemCategory::Material;
        return true;
    }
    else if (Value == TEXT("Quest"))
    {
        Out = EItemCategory::Quest;
        return true;
    }
    else if (Value == TEXT("Currency"))
    {
        Out = EItemCategory::Currency;
        return true;
    }
    return false;
}

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Normal UMETA(DisplayName = "Normal"),
    Uncommon UMETA(DisplayName = "Uncommon"),
    Rare UMETA(DisplayName = "Rare"),
    Epic UMETA(DisplayName = "Epic")
};

inline bool TryParseEItemRarity(const FString& Value, EItemRarity& Out)
{
    if (Value == TEXT("Normal"))
    {
        Out = EItemRarity::Normal;
        return true;
    }
    else if (Value == TEXT("Uncommon"))
    {
        Out = EItemRarity::Uncommon;
        return true;
    }
    else if (Value == TEXT("Rare"))
    {
        Out = EItemRarity::Rare;
        return true;
    }
    else if (Value == TEXT("Epic"))
    {
        Out = EItemRarity::Epic;
        return true;
    }
    return false;
}

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
    None UMETA(DisplayName = "None"),
    Weapon UMETA(DisplayName = "Weapon"),
    Chest UMETA(DisplayName = "Chest"),
    Legs UMETA(DisplayName = "Legs"),
    Hands UMETA(DisplayName = "Hands"),
    Necklace UMETA(DisplayName = "Necklace"),
    RingLeft UMETA(DisplayName = "RingLeft"),
    RingRight UMETA(DisplayName = "RingRight")
};

inline bool TryParseEEquipSlot(const FString& Value, EEquipSlot& Out)
{
    if (Value == TEXT("None"))
    {
        Out = EEquipSlot::None;
        return true;
    }
    else if (Value == TEXT("Weapon"))
    {
        Out = EEquipSlot::Weapon;
        return true;
    }
    else if (Value == TEXT("Chest"))
    {
        Out = EEquipSlot::Chest;
        return true;
    }
    else if (Value == TEXT("Legs"))
    {
        Out = EEquipSlot::Legs;
        return true;
    }
    else if (Value == TEXT("Hands"))
    {
        Out = EEquipSlot::Hands;
        return true;
    }
    else if (Value == TEXT("Necklace"))
    {
        Out = EEquipSlot::Necklace;
        return true;
    }
    else if (Value == TEXT("RingLeft"))
    {
        Out = EEquipSlot::RingLeft;
        return true;
    }
    else if (Value == TEXT("RingRight"))
    {
        Out = EEquipSlot::RingRight;
        return true;
    }
    return false;
}

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None UMETA(DisplayName = "None"),
    SwordShield UMETA(DisplayName = "SwordShield"),
    Greatsword UMETA(DisplayName = "Greatsword"),
    Staff UMETA(DisplayName = "Staff")
};

inline bool TryParseEWeaponType(const FString& Value, EWeaponType& Out)
{
    if (Value == TEXT("None"))
    {
        Out = EWeaponType::None;
        return true;
    }
    else if (Value == TEXT("SwordShield"))
    {
        Out = EWeaponType::SwordShield;
        return true;
    }
    else if (Value == TEXT("Greatsword"))
    {
        Out = EWeaponType::Greatsword;
        return true;
    }
    else if (Value == TEXT("Staff"))
    {
        Out = EWeaponType::Staff;
        return true;
    }
    return false;
}

// ============================================================
// Row Struct
// ============================================================
USTRUCT(BlueprintType)
struct FItemTableRow
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 ItemId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString ItemCode{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString ItemName{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    EItemCategory ItemCategory = EItemCategory::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    EItemRarity Rarity = EItemRarity::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    EEquipSlot EquipSlot = EEquipSlot::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    EWeaponType WeaponType = EWeaponType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 MaxStackCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 MaxDurability = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 MaxTradeCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 SellValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    bool bCanUseSharedStorage = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    bool bCanBindToConsumableQuickSlot = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString IconKey{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    int32 BalanceVersion = 0;
};

// ============================================================
// JSON Loaders (inline)
// ============================================================
inline bool LoadItemTableRow(const TSharedPtr<FJsonObject>& Object,
                               FItemTableRow& OutRow, FString& OutError)
{
    if (!Object.IsValid())
    {
        OutError = TEXT("table row must be a JSON object");
        return false;
    }

    {
        double Value = 0.0;
        if (!Object->TryGetNumberField(TEXT("ItemId"), Value))
        {
            OutError = FString::Printf(TEXT("missing required numeric field: ItemTable.ItemId"));
            return false;
        }
        OutRow.ItemId = static_cast<int32>(Value);
    }
    {
        FString Value;
        if (!Object->TryGetStringField(TEXT("ItemCode"), Value))
        {
            OutError = FString::Printf(TEXT("missing required string field: ItemTable.ItemCode"));
            return false;
        }
        OutRow.ItemCode = MoveTemp(Value);
    }
    {
        FString Value;
        if (!Object->TryGetStringField(TEXT("ItemName"), Value))
        {
            OutError = FString::Printf(TEXT("missing required string field: ItemTable.ItemName"));
            return false;
        }
        OutRow.ItemName = MoveTemp(Value);
    }
    {
        FString Value;
        if (!Object->TryGetStringField(TEXT("ItemCategory"), Value))
        {
            OutError = FString::Printf(TEXT("missing required enum field: ItemTable.ItemCategory"));
            return false;
        }
        if (!TryParseEItemCategory(Value, OutRow.ItemCategory))
        {
            OutError = FString::Printf(TEXT("failed to parse enum field: ItemTable.ItemCategory"));
            return false;
        }
    }
    {
        FString Value;
        if (!Object->TryGetStringField(TEXT("Rarity"), Value))
        {
            OutError = FString::Printf(TEXT("missing required enum field: ItemTable.Rarity"));
            return false;
        }
        if (!TryParseEItemRarity(Value, OutRow.Rarity))
        {
            OutError = FString::Printf(TEXT("failed to parse enum field: ItemTable.Rarity"));
            return false;
        }
    }
    {
        FString Value;
        if (!Object->TryGetStringField(TEXT("EquipSlot"), Value))
        {
            OutError = FString::Printf(TEXT("missing required enum field: ItemTable.EquipSlot"));
            return false;
        }
        if (!TryParseEEquipSlot(Value, OutRow.EquipSlot))
        {
            OutError = FString::Printf(TEXT("failed to parse enum field: ItemTable.EquipSlot"));
            return false;
        }
    }
    {
        FString Value;
        if (!Object->TryGetStringField(TEXT("WeaponType"), Value))
        {
            OutError = FString::Printf(TEXT("missing required enum field: ItemTable.WeaponType"));
            return false;
        }
        if (!TryParseEWeaponType(Value, OutRow.WeaponType))
        {
            OutError = FString::Printf(TEXT("failed to parse enum field: ItemTable.WeaponType"));
            return false;
        }
    }
    {
        double Value = 0.0;
        if (!Object->TryGetNumberField(TEXT("MaxStackCount"), Value))
        {
            OutError = FString::Printf(TEXT("missing required numeric field: ItemTable.MaxStackCount"));
            return false;
        }
        OutRow.MaxStackCount = static_cast<int32>(Value);
    }
    {
        double Value = 0.0;
        if (!Object->TryGetNumberField(TEXT("MaxDurability"), Value))
        {
            OutError = FString::Printf(TEXT("missing required numeric field: ItemTable.MaxDurability"));
            return false;
        }
        OutRow.MaxDurability = static_cast<int32>(Value);
    }
    {
        double Value = 0.0;
        if (!Object->TryGetNumberField(TEXT("MaxTradeCount"), Value))
        {
            OutError = FString::Printf(TEXT("missing required numeric field: ItemTable.MaxTradeCount"));
            return false;
        }
        OutRow.MaxTradeCount = static_cast<int32>(Value);
    }
    {
        double Value = 0.0;
        if (!Object->TryGetNumberField(TEXT("SellValue"), Value))
        {
            OutError = FString::Printf(TEXT("missing required numeric field: ItemTable.SellValue"));
            return false;
        }
        OutRow.SellValue = static_cast<int32>(Value);
    }
    {
        bool Value = false;
        if (!Object->TryGetBoolField(TEXT("bCanUseSharedStorage"), Value))
        {
            OutError = FString::Printf(TEXT("missing required bool field: ItemTable.bCanUseSharedStorage"));
            return false;
        }
        OutRow.bCanUseSharedStorage = Value;
    }
    {
        bool Value = false;
        if (!Object->TryGetBoolField(TEXT("bCanBindToConsumableQuickSlot"), Value))
        {
            OutError = FString::Printf(TEXT("missing required bool field: ItemTable.bCanBindToConsumableQuickSlot"));
            return false;
        }
        OutRow.bCanBindToConsumableQuickSlot = Value;
    }
    {
        FString Value;
        if (!Object->TryGetStringField(TEXT("IconKey"), Value))
        {
            OutError = FString::Printf(TEXT("missing required string field: ItemTable.IconKey"));
            return false;
        }
        OutRow.IconKey = MoveTemp(Value);
    }
    {
        double Value = 0.0;
        if (!Object->TryGetNumberField(TEXT("BalanceVersion"), Value))
        {
            OutError = FString::Printf(TEXT("missing required numeric field: ItemTable.BalanceVersion"));
            return false;
        }
        OutRow.BalanceVersion = static_cast<int32>(Value);
    }
    return true;
}

inline bool LoadItemTable(const FString& JsonText,
                            TArray<FItemTableRow>& OutRows, FString& OutError)
{
    TArray<TSharedPtr<FJsonValue>> RootArray;
    {
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
        if (FJsonSerializer::Deserialize(Reader, RootArray))
        {
            OutRows.Reset(RootArray.Num());
            for (const TSharedPtr<FJsonValue>& Entry : RootArray)
            {
                const TSharedPtr<FJsonObject> EntryObject = Entry.IsValid() ? Entry->AsObject() : nullptr;
                if (!EntryObject.IsValid())
                {
                    OutError = TEXT("table row must be a JSON object");
                    return false;
                }

                FItemTableRow Row{};
                if (!LoadItemTableRow(EntryObject, Row, OutError))
                {
                    return false;
                }
                OutRows.Add(MoveTemp(Row));
            }
            return true;
        }
    }

    TSharedPtr<FJsonObject> RootObject;
    {
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
        if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
        {
            OutError = TEXT("failed to parse table JSON");
            return false;
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;
    if (!RootObject->TryGetArrayField(TEXT("rows"), Rows) && !RootObject->TryGetArrayField(TEXT("data"), Rows))
    {
        OutError = TEXT("table root must be an array or an object with rows/data");
        return false;
    }

    OutRows.Reset(Rows->Num());
    for (const TSharedPtr<FJsonValue>& Entry : *Rows)
    {
        const TSharedPtr<FJsonObject> EntryObject = Entry.IsValid() ? Entry->AsObject() : nullptr;
        if (!EntryObject.IsValid())
        {
            OutError = TEXT("table row must be a JSON object");
            return false;
        }

        FItemTableRow Row{};
        if (!LoadItemTableRow(EntryObject, Row, OutError))
        {
            return false;
        }
        OutRows.Add(MoveTemp(Row));
    }
    return true;
}

inline bool LoadItemTableFromFile(const FString& FilePath,
                                    TArray<FItemTableRow>& OutRows, FString& OutError)
{
    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *FilePath))
    {
        OutError = FString::Printf(TEXT("failed to read data file: %s"), *FilePath);
        return false;
    }
    return LoadItemTable(JsonText, OutRows, OutError);
}

// ============================================================
// Index Builders
// ============================================================
inline TMap<int32, FItemTableRow> BuildItemTableIndexByItemId(
    const TArray<FItemTableRow>& Rows)
{
    TMap<int32, FItemTableRow> Index;
    for (const auto& Row : Rows)
    {
        Index.Add(Row.ItemId, Row);
    }
    return Index;
}
