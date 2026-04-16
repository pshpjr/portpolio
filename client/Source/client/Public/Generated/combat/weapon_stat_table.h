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
#include "weapon_stat_table.generated.h"
USTRUCT(BlueprintType)
struct FWeaponStatTableRow
{
    GENERATED_BODY()
public:
    int32 WeaponStatTableId{};

    int32 EnhanceLevel{};

    int32 WeaponPower{};

    float BaseMoveSpeed{};

    float AttackSpeed{};

    float CastSpeed{};

    int32 IdentityGaugeMax{};

    float IdentityGaugeGain{};

    EResourceType ResourceType{};

    int32 ResourceMax{};

    int32 ResourceRegen{};

    int32 StaggerPower{};

    float ThreatGen{};

    float ParryWindowBonus{};

    int32 BalanceVersion{};
};

struct WeaponStatTableSchema
{
    static constexpr const TCHAR* TableName = TEXT("WeaponStatTable");
    static constexpr const TCHAR* SchemaFile = TEXT("weapon_stat_table.schema.json");
    static constexpr const TCHAR* DataFile = TEXT("weapon_stat_table.data.json");
    static constexpr int32 RowCount = 75;
    static constexpr int32 FieldCount = 15;
    static constexpr FTableFieldMetadata Fields[FieldCount] = {
        FTableFieldMetadata{TEXT("WeaponStatTableId"), TEXT("int32"), true, true},
        FTableFieldMetadata{TEXT("EnhanceLevel"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("WeaponPower"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("BaseMoveSpeed"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("AttackSpeed"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("CastSpeed"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("IdentityGaugeMax"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("IdentityGaugeGain"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("ResourceType"), TEXT("enum"), true, false},
        FTableFieldMetadata{TEXT("ResourceMax"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("ResourceRegen"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("StaggerPower"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("ThreatGen"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("ParryWindowBonus"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("BalanceVersion"), TEXT("int32"), true, false},
    };
};

inline bool LoadWeaponStatTableRow(const TSharedPtr<FJsonObject>& object, FWeaponStatTableRow& out_row, FString& out_error)
{
    if (!object.IsValid())
    {
        out_error = TEXT("table row must be a JSON object");
        return false;
    }

    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("WeaponStatTableId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.WeaponStatTableId"));
            return false;
        }
        out_row.WeaponStatTableId = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("EnhanceLevel"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.EnhanceLevel"));
            return false;
        }
        out_row.EnhanceLevel = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("WeaponPower"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.WeaponPower"));
            return false;
        }
        out_row.WeaponPower = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BaseMoveSpeed"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.BaseMoveSpeed"));
            return false;
        }
        out_row.BaseMoveSpeed = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("AttackSpeed"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.AttackSpeed"));
            return false;
        }
        out_row.AttackSpeed = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("CastSpeed"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.CastSpeed"));
            return false;
        }
        out_row.CastSpeed = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("IdentityGaugeMax"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.IdentityGaugeMax"));
            return false;
        }
        out_row.IdentityGaugeMax = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("IdentityGaugeGain"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.IdentityGaugeGain"));
            return false;
        }
        out_row.IdentityGaugeGain = static_cast<float>(value);
    }
    {
        FString value;
        if (!object->TryGetStringField(TEXT("ResourceType"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: WeaponStatTable.ResourceType"));
            return false;
        }
        EResourceType parsed_value{};
        if (!TryParseEResourceType(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: WeaponStatTable.ResourceType"));
            return false;
        }
        out_row.ResourceType = parsed_value;
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("ResourceMax"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.ResourceMax"));
            return false;
        }
        out_row.ResourceMax = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("ResourceRegen"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.ResourceRegen"));
            return false;
        }
        out_row.ResourceRegen = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("StaggerPower"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.StaggerPower"));
            return false;
        }
        out_row.StaggerPower = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("ThreatGen"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.ThreatGen"));
            return false;
        }
        out_row.ThreatGen = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("ParryWindowBonus"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.ParryWindowBonus"));
            return false;
        }
        out_row.ParryWindowBonus = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BalanceVersion"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponStatTable.BalanceVersion"));
            return false;
        }
        out_row.BalanceVersion = static_cast<int32>(value);
    }
    return true;
}

inline bool LoadWeaponStatTable(const FString& json_text, TArray<FWeaponStatTableRow>& out_rows, FString& out_error)
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

        FWeaponStatTableRow row{};
        if (!LoadWeaponStatTableRow(*entry_object, row, out_error))
        {
            return false;
        }
        out_rows.Add(MoveTemp(row));
    }
    return true;
}

inline bool LoadWeaponStatTableFromFile(const FString& file_path, TArray<FWeaponStatTableRow>& out_rows, FString& out_error)
{
    FString json_text;
    if (!FFileHelper::LoadFileToString(json_text, *file_path))
    {
        out_error = FString::Printf(TEXT("failed to read data file: %s"), *file_path);
        return false;
    }
    return LoadWeaponStatTable(json_text, out_rows, out_error);
}
