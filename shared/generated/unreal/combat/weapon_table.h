// Auto-generated. Do not edit.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "combat_data_common.h"
#include "combat_data_enums.h"

struct FWeaponTableRow
{
    int32 WeaponId{};

    FString WeaponCode{};

    FString WeaponName{};

    EWeaponType WeaponType{};

    int32 BaseWeaponStatTableId{};

    int32 MaxEnhanceLevel{};

    int32 SkillSetId{};

    int32 IdentitySkillId{};

    FString SmartDropTag{};

    int32 OptionPoolId{};

    int32 BaseDurabilityMax{};

    float RepairCostRate{};

    int32 EquipLevelMin{};

    int32 TradeLimitCount{};

    int32 AnimationSetId{};

    FString IconKey{};

    FString ModelKey{};

    int32 DisplayOrder{};

    EWeaponHandType HandType{};

    FString CombatRoleTag{};

    ERangeProfile RangeProfile{};

    int32 BalanceVersion{};
};

struct WeaponTableSchema
{
    static constexpr const TCHAR* TableName = TEXT("WeaponTable");
    static constexpr const TCHAR* SchemaFile = TEXT("weapon_table.schema.json");
    static constexpr const TCHAR* DataFile = TEXT("weapon_table.data.json");
    static constexpr int32 RowCount = 3;
    static constexpr int32 FieldCount = 22;
    static constexpr FTableFieldMetadata Fields[FieldCount] = {
        FTableFieldMetadata{TEXT("WeaponId"), TEXT("int32"), true, true},
        FTableFieldMetadata{TEXT("WeaponCode"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("WeaponName"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("WeaponType"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("BaseWeaponStatTableId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("MaxEnhanceLevel"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("SkillSetId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("IdentitySkillId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("SmartDropTag"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("OptionPoolId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("BaseDurabilityMax"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("RepairCostRate"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("EquipLevelMin"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("TradeLimitCount"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("AnimationSetId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("IconKey"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("ModelKey"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("DisplayOrder"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("HandType"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("CombatRoleTag"), TEXT("string"), true, false},
        FTableFieldMetadata{TEXT("RangeProfile"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("BalanceVersion"), TEXT("int32"), true, false},
    };
};

inline bool LoadWeaponTableRow(const TSharedPtr<FJsonObject>& object, FWeaponTableRow& out_row, FString& out_error)
{
    if (!object.IsValid())
    {
        out_error = TEXT("table row must be a JSON object");
        return false;
    }

    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("WeaponId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.WeaponId"));
            return false;
        }
        out_row.WeaponId = static_cast<int32>(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("WeaponCode"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: WeaponTable.WeaponCode"));
            return false;
        }
        out_row.WeaponCode = MoveTemp(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("WeaponName"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: WeaponTable.WeaponName"));
            return false;
        }
        out_row.WeaponName = MoveTemp(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("WeaponType"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: WeaponTable.WeaponType"));
            return false;
        }
        EWeaponType parsed_value{};
        if (!TryParseEWeaponType(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: WeaponTable.WeaponType"));
            return false;
        }
        out_row.WeaponType = parsed_value;
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BaseWeaponStatTableId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.BaseWeaponStatTableId"));
            return false;
        }
        out_row.BaseWeaponStatTableId = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("MaxEnhanceLevel"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.MaxEnhanceLevel"));
            return false;
        }
        out_row.MaxEnhanceLevel = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("SkillSetId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.SkillSetId"));
            return false;
        }
        out_row.SkillSetId = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("IdentitySkillId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.IdentitySkillId"));
            return false;
        }
        out_row.IdentitySkillId = static_cast<int32>(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("SmartDropTag"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: WeaponTable.SmartDropTag"));
            return false;
        }
        out_row.SmartDropTag = MoveTemp(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("OptionPoolId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.OptionPoolId"));
            return false;
        }
        out_row.OptionPoolId = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BaseDurabilityMax"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.BaseDurabilityMax"));
            return false;
        }
        out_row.BaseDurabilityMax = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("RepairCostRate"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.RepairCostRate"));
            return false;
        }
        out_row.RepairCostRate = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("EquipLevelMin"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.EquipLevelMin"));
            return false;
        }
        out_row.EquipLevelMin = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("TradeLimitCount"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.TradeLimitCount"));
            return false;
        }
        out_row.TradeLimitCount = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("AnimationSetId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.AnimationSetId"));
            return false;
        }
        out_row.AnimationSetId = static_cast<int32>(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("IconKey"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: WeaponTable.IconKey"));
            return false;
        }
        out_row.IconKey = MoveTemp(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("ModelKey"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: WeaponTable.ModelKey"));
            return false;
        }
        out_row.ModelKey = MoveTemp(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("DisplayOrder"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.DisplayOrder"));
            return false;
        }
        out_row.DisplayOrder = static_cast<int32>(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("HandType"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: WeaponTable.HandType"));
            return false;
        }
        EWeaponHandType parsed_value{};
        if (!TryParseEWeaponHandType(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: WeaponTable.HandType"));
            return false;
        }
        out_row.HandType = parsed_value;
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("CombatRoleTag"), value))
        {
            out_error = FString::Printf(TEXT("missing required string field: WeaponTable.CombatRoleTag"));
            return false;
        }
        out_row.CombatRoleTag = MoveTemp(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("RangeProfile"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: WeaponTable.RangeProfile"));
            return false;
        }
        ERangeProfile parsed_value{};
        if (!TryParseERangeProfile(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: WeaponTable.RangeProfile"));
            return false;
        }
        out_row.RangeProfile = parsed_value;
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BalanceVersion"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponTable.BalanceVersion"));
            return false;
        }
        out_row.BalanceVersion = static_cast<int32>(value);
    }
    return true;
}

inline bool LoadWeaponTable(const FString& json_text, TArray<FWeaponTableRow>& out_rows, FString& out_error)
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

        FWeaponTableRow row{};
        if (!LoadWeaponTableRow(*entry_object, row, out_error))
        {
            return false;
        }
        out_rows.Add(MoveTemp(row));
    }
    return true;
}

inline bool LoadWeaponTableFromFile(const FString& file_path, TArray<FWeaponTableRow>& out_rows, FString& out_error)
{
    FString json_text;
    if (!FFileHelper::LoadFileToString(json_text, *file_path))
    {
        out_error = FString::Printf(TEXT("failed to read data file: %s"), *file_path);
        return false;
    }
    return LoadWeaponTable(json_text, out_rows, out_error);
}
