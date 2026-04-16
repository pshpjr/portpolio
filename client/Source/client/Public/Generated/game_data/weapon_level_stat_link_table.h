// Auto-generated. Do not edit.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "table_common.h"
#include "enums.h"
#include "weapon_level_stat_link_table.generated.h"

USTRUCT(BlueprintType)
struct FWeaponLevelStatLinkTableRow
{
    GENERATED_BODY()
public:
    EWeaponType WeaponType{};

    int32 EnhanceLevel{};

    int32 WeaponStatTableId{};

    int32 BalanceVersion{};
};

struct WeaponLevelStatLinkTableSchema
{
    static constexpr const TCHAR* TableName = TEXT("WeaponLevelStatLinkTable");
    static constexpr const TCHAR* SchemaFile = TEXT("weapon_level_stat_link_table.schema.json");
    static constexpr const TCHAR* DataFile = TEXT("weapon_level_stat_link_table.data.json");
    static constexpr int32 RowCount = 75;
    static constexpr int32 FieldCount = 4;
    static constexpr FTableFieldMetadata Fields[FieldCount] = {
        FTableFieldMetadata{TEXT("WeaponType"), TEXT("enum"), true, true},
        FTableFieldMetadata{TEXT("EnhanceLevel"), TEXT("int32"), true, true},
        FTableFieldMetadata{TEXT("WeaponStatTableId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("BalanceVersion"), TEXT("int32"), true, true},
    };
};

inline bool LoadWeaponLevelStatLinkTableRow(const TSharedPtr<FJsonObject>& object, FWeaponLevelStatLinkTableRow& out_row, FString& out_error)
{
    if (!object.IsValid())
    {
        out_error = TEXT("table row must be a JSON object");
        return false;
    }

    {
        FString value;
        if (!object->TryGetStringField(TEXT("WeaponType"), value))
        {
            out_error = FString::Printf(TEXT("missing required enum field: WeaponLevelStatLinkTable.WeaponType"));
            return false;
        }
        EWeaponType parsed_value{};
        if (!TryParseEWeaponType(value, parsed_value))
        {
            out_error = FString::Printf(TEXT("failed to parse enum field: WeaponLevelStatLinkTable.WeaponType"));
            return false;
        }
        out_row.WeaponType = parsed_value;
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("EnhanceLevel"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponLevelStatLinkTable.EnhanceLevel"));
            return false;
        }
        out_row.EnhanceLevel = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("WeaponStatTableId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponLevelStatLinkTable.WeaponStatTableId"));
            return false;
        }
        out_row.WeaponStatTableId = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BalanceVersion"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: WeaponLevelStatLinkTable.BalanceVersion"));
            return false;
        }
        out_row.BalanceVersion = static_cast<int32>(value);
    }
    return true;
}

inline bool LoadWeaponLevelStatLinkTable(const FString& json_text, TArray<FWeaponLevelStatLinkTableRow>& out_rows, FString& out_error)
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

        FWeaponLevelStatLinkTableRow row{};
        if (!LoadWeaponLevelStatLinkTableRow(*entry_object, row, out_error))
        {
            return false;
        }
        out_rows.Add(MoveTemp(row));
    }
    return true;
}

inline bool LoadWeaponLevelStatLinkTableFromFile(const FString& file_path, TArray<FWeaponLevelStatLinkTableRow>& out_rows, FString& out_error)
{
    FString json_text;
    if (!FFileHelper::LoadFileToString(json_text, *file_path))
    {
        out_error = FString::Printf(TEXT("failed to read data file: %s"), *file_path);
        return false;
    }
    return LoadWeaponLevelStatLinkTable(json_text, out_rows, out_error);
}