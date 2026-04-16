// Auto-generated. Do not edit.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "../combat_data_common.h"
#include "../combat_data_enums.h"
#include "item_table.generated.h"
USTRUCT(BlueprintType)
struct FItemTableRow
{
    GENERATED_BODY()
public:
    int32 ItemId{};

    FString ItemCode{};

    FString ItemName{};

    EItemCategory ItemCategory{};

    EItemRarity Rarity{};

    EEquipSlot EquipSlot{};

    EWeaponType WeaponType{};

    int32 MaxStackCount{};

    int32 MaxDurability{};

    int32 MaxTradeCount{};

    int32 SellValue{};

    bool bCanUseSharedStorage{};

    bool bCanBindToConsumableQuickSlot{};

    FString IconKey{};

    int32 BalanceVersion{};
};

struct ItemTableSchema
{
    static constexpr const TCHAR* TableName = TEXT("ItemTable");
    static constexpr const TCHAR* SchemaFile = TEXT("item_table.schema.json");
    static constexpr const TCHAR* DataFile = TEXT("item_table.data.json");
    static constexpr int32 RowCount = 5;
    static constexpr int32 FieldCount = 15;
    static constexpr FTableFieldMetadata Fields[FieldCount] = {
        FTableFieldMetadata{TEXT("ItemId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("ItemCode"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("ItemName"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("ItemCategory"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("Rarity"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("EquipSlot"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("WeaponType"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("MaxStackCount"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("MaxDurability"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("MaxTradeCount"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("SellValue"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("bCanUseSharedStorage"), TEXT("bool"), true, false},
        FTableFieldMetadata{TEXT("bCanBindToConsumableQuickSlot"), TEXT("bool"), true, false},
        FTableFieldMetadata{TEXT("IconKey"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("BalanceVersion"), TEXT("int32"), true, false},
    };
};

inline bool LoadItemTableRow(const TSharedPtr<FJsonObject>& object, FItemTableRow& out_row, FString& out_error)
{
    if (!object.IsValid())
    {
        out_error = TEXT("table row must be a JSON object");
        return false;
    }

    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("ItemId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: ItemTable.ItemId"));
            return false;
        }
        out_row.ItemId = static_cast<int32>(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("ItemCode"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: ItemTable.ItemCode"));
            return false;
        }
        out_row.ItemCode = MoveTemp(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("ItemName"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: ItemTable.ItemName"));
            return false;
        }
        out_row.ItemName = MoveTemp(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("ItemCategory"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: ItemTable.ItemCategory"));
            return false;
        }
        EItemCategory parsed_value{};
        if (!TryParseEItemCategory(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: ItemTable.ItemCategory"));
            return false;
        }
        out_row.ItemCategory = parsed_value;
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("Rarity"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: ItemTable.Rarity"));
            return false;
        }
        EItemRarity parsed_value{};
        if (!TryParseEItemRarity(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: ItemTable.Rarity"));
            return false;
        }
        out_row.Rarity = parsed_value;
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("EquipSlot"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: ItemTable.EquipSlot"));
            return false;
        }
        EEquipSlot parsed_value{};
        if (!TryParseEEquipSlot(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: ItemTable.EquipSlot"));
            return false;
        }
        out_row.EquipSlot = parsed_value;
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("WeaponType"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: ItemTable.WeaponType"));
            return false;
        }
        EWeaponType parsed_value{};
        if (!TryParseEWeaponType(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: ItemTable.WeaponType"));
            return false;
        }
        out_row.WeaponType = parsed_value;
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("MaxStackCount"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: ItemTable.MaxStackCount"));
            return false;
        }
        out_row.MaxStackCount = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("MaxDurability"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: ItemTable.MaxDurability"));
            return false;
        }
        out_row.MaxDurability = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("MaxTradeCount"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: ItemTable.MaxTradeCount"));
            return false;
        }
        out_row.MaxTradeCount = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("SellValue"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: ItemTable.SellValue"));
            return false;
        }
        out_row.SellValue = static_cast<int32>(value);
    }
    {
        bool value = false;
        if (!object->TryGetBoolField(TEXT("bCanUseSharedStorage"), value))
        {
            out_error = FString::Printf(TEXT("missing required bool field: ItemTable.bCanUseSharedStorage"));
            return false;
        }
        out_row.bCanUseSharedStorage = value;
    }
    {
        bool value = false;
        if (!object->TryGetBoolField(TEXT("bCanBindToConsumableQuickSlot"), value))
        {
            out_error = FString::Printf(TEXT("missing required bool field: ItemTable.bCanBindToConsumableQuickSlot"));
            return false;
        }
        out_row.bCanBindToConsumableQuickSlot = value;
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("IconKey"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: ItemTable.IconKey"));
            return false;
        }
        out_row.IconKey = MoveTemp(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BalanceVersion"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: ItemTable.BalanceVersion"));
            return false;
        }
        out_row.BalanceVersion = static_cast<int32>(value);
    }
    return true;
}

inline bool LoadItemTable(const FString& json_text, TArray<FItemTableRow>& out_rows, FString& out_error)
{
    TSharedPtr<FJsonObject> root_object;
    TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(json_text);
    if (!FJsonSerializer::Deserialize(reader, root_object) || !root_object.IsValid())
    {
        out_error = TEXT("failed to parse table JSON");
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* rows = nullptr;
    if (!root_object->TryGetArrayField(TEXT("rows"), rows))
    {
        if (!root_object->TryGetArrayField(TEXT("data"), rows))
        {
            out_error = TEXT("table root must expose a \"rows\" or \"data\" array");
            return false;
        }
    }

    out_rows.Reset(rows->Num());
    for (const TSharedPtr<FJsonValue>& entry : *rows)
    {
        const TSharedPtr<FJsonObject>* entry_object = nullptr;
        if (!entry.IsValid() || entry->Type != EJson::Object || !entry->TryGetObject(entry_object) || !entry_object || !entry_object->IsValid())
        {
            out_error = TEXT("table row must be a JSON object");
            return false;
        }

        FItemTableRow row{};
        if (!LoadItemTableRow(*entry_object, row, out_error))
        {
            return false;
        }
        out_rows.Add(MoveTemp(row));
    }
    return true;
}

inline bool LoadItemTableFromFile(const FString& file_path, TArray<FItemTableRow>& out_rows, FString& out_error)
{
    FString json_text;
    if (!FFileHelper::LoadFileToString(json_text, *file_path))
    {
        out_error = FString::Printf(TEXT("failed to read data file: %s"), *file_path);
        return false;
    }
    return LoadItemTable(json_text, out_rows, out_error);
}
