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
#include "user_level_stat_link_table.generated.h"

USTRUCT(BlueprintType)
struct FUserLevelStatLinkTableRow
{
    GENERATED_BODY()
public:
    int32 Level{};

    int32 PlayerStatTableId{};

    int32 BalanceVersion{};
};

struct UserLevelStatLinkTableSchema
{
    static constexpr const TCHAR* TableName = TEXT("UserLevelStatLinkTable");
    static constexpr const TCHAR* SchemaFile = TEXT("user_level_stat_link_table.schema.json");
    static constexpr const TCHAR* DataFile = TEXT("user_level_stat_link_table.data.json");
    static constexpr int32 RowCount = 40;
    static constexpr int32 FieldCount = 3;
    static constexpr FTableFieldMetadata Fields[FieldCount] = {
        FTableFieldMetadata{TEXT("Level"), TEXT("int32"), true, true},
        FTableFieldMetadata{TEXT("PlayerStatTableId"), TEXT("int32"), true, false},
        FTableFieldMetadata{TEXT("BalanceVersion"), TEXT("int32"), true, true},
    };
};

inline bool LoadUserLevelStatLinkTableRow(const TSharedPtr<FJsonObject>& object, FUserLevelStatLinkTableRow& out_row, FString& out_error)
{
    if (!object.IsValid())
    {
        out_error = TEXT("table row must be a JSON object");
        return false;
    }

    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("Level"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: UserLevelStatLinkTable.Level"));
            return false;
        }
        out_row.Level = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("PlayerStatTableId"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: UserLevelStatLinkTable.PlayerStatTableId"));
            return false;
        }
        out_row.PlayerStatTableId = static_cast<int32>(value);
    }
    {
        double value = 0.0;
        if (!object->TryGetNumberField(TEXT("BalanceVersion"), value))
        {
            out_error = FString::Printf(TEXT("missing required numeric field: UserLevelStatLinkTable.BalanceVersion"));
            return false;
        }
        out_row.BalanceVersion = static_cast<int32>(value);
    }
    return true;
}

inline bool LoadUserLevelStatLinkTable(const FString& json_text, TArray<FUserLevelStatLinkTableRow>& out_rows, FString& out_error)
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

        FUserLevelStatLinkTableRow row{};
        if (!LoadUserLevelStatLinkTableRow(*entry_object, row, out_error))
        {
            return false;
        }
        out_rows.Add(MoveTemp(row));
    }
    return true;
}

inline bool LoadUserLevelStatLinkTableFromFile(const FString& file_path, TArray<FUserLevelStatLinkTableRow>& out_rows, FString& out_error)
{
    FString json_text;
    if (!FFileHelper::LoadFileToString(json_text, *file_path))
    {
        out_error = FString::Printf(TEXT("failed to read data file: %s"), *file_path);
        return false;
    }
    return LoadUserLevelStatLinkTable(json_text, out_rows, out_error);
}