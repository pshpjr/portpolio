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
#include "player_stat_table.generated.h"
USTRUCT(BlueprintType)
struct FPlayerStatTableRow
{
    GENERATED_BODY()
public:
    int32 PlayerStatTableId{};

    int32 Level{};

    int32 MaxHP{};

    int32 AttackPower{};

    int32 Defense{};

    float CriticalChance{};

    float CriticalDamage{};

    float CooldownReduction{};

    float DamageReduction{};

    float MoveSpeedBonus{};

    int32 BalanceVersion{};
};

struct PlayerStatTableSchema
{
    static constexpr const TCHAR* TableName = TEXT("PlayerStatTable");
    static constexpr const TCHAR* SchemaFile = TEXT("player_stat_table.schema.json");
    static constexpr const TCHAR* DataFile = TEXT("player_stat_table.data.json");
    static constexpr int32 RowCount = 40;
    static constexpr int32 FieldCount = 11;
    static constexpr FTableFieldMetadata Fields[FieldCount] = {
        FTableFieldMetadata{TEXT("PlayerStatTableId"), TEXT("int32"), true, true},
        FTableFieldMetadata{TEXT("Level"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("MaxHP"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("AttackPower"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("Defense"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("CriticalChance"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("CriticalDamage"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("CooldownReduction"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("DamageReduction"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("MoveSpeedBonus"), TEXT("float"), true, false},
        FTableFieldMetadata{TEXT("BalanceVersion"), TEXT("int32"), true, false},
    };
};

inline bool LoadPlayerStatTableRow(const TSharedPtr<FJsonObject>& object, FPlayerStatTableRow& out_row, FString& out_error)
{
    if (!object.IsValid())
    {
        out_error = TEXT("table row must be a JSON object");
        return false;
    }

    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("PlayerStatTableId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.PlayerStatTableId"));
            return false;
        }
        out_row.PlayerStatTableId = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("Level"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.Level"));
            return false;
        }
        out_row.Level = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("MaxHP"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.MaxHP"));
            return false;
        }
        out_row.MaxHP = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("AttackPower"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.AttackPower"));
            return false;
        }
        out_row.AttackPower = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("Defense"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.Defense"));
            return false;
        }
        out_row.Defense = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("CriticalChance"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.CriticalChance"));
            return false;
        }
        out_row.CriticalChance = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("CriticalDamage"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.CriticalDamage"));
            return false;
        }
        out_row.CriticalDamage = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("CooldownReduction"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.CooldownReduction"));
            return false;
        }
        out_row.CooldownReduction = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("DamageReduction"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.DamageReduction"));
            return false;
        }
        out_row.DamageReduction = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("MoveSpeedBonus"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.MoveSpeedBonus"));
            return false;
        }
        out_row.MoveSpeedBonus = static_cast<float>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BalanceVersion"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: PlayerStatTable.BalanceVersion"));
            return false;
        }
        out_row.BalanceVersion = static_cast<int32>(value);
    }
    return true;
}

inline bool LoadPlayerStatTable(const FString& json_text, TArray<FPlayerStatTableRow>& out_rows, FString& out_error)
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

        FPlayerStatTableRow row{};
        if (!LoadPlayerStatTableRow(*entry_object, row, out_error))
        {
            return false;
        }
        out_rows.Add(MoveTemp(row));
    }
    return true;
}

inline bool LoadPlayerStatTableFromFile(const FString& file_path, TArray<FPlayerStatTableRow>& out_rows, FString& out_error)
{
    FString json_text;
    if (!FFileHelper::LoadFileToString(json_text, *file_path))
    {
        out_error = FString::Printf(TEXT("failed to read data file: %s"), *file_path);
        return false;
    }
    return LoadPlayerStatTable(json_text, out_rows, out_error);
}
