// Auto-generated. Do not edit.
#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "combat_data_common.h"
#include "combat_data_enums.h"

namespace psh::generated::combat {

struct UserLevelStatLinkTableRow
{
    std::int32_t Level{};
    std::int32_t PlayerStatTableId{};
    std::int32_t BalanceVersion{};
};

struct UserLevelStatLinkTableSchema
{
    static constexpr std::string_view TableName = "UserLevelStatLinkTable";
    static constexpr std::string_view SchemaFile = "user_level_stat_link_table.schema.json";
    static constexpr std::string_view DataFile = "user_level_stat_link_table.data.json";
    static constexpr std::size_t RowCount = 40;
    static constexpr std::size_t FieldCount = 3;
    static constexpr std::array<TableFieldMetadata, FieldCount> Fields = {
        TableFieldMetadata{"Level", "int32", true, true},
        TableFieldMetadata{"PlayerStatTableId", "int32", true, false},
        TableFieldMetadata{"BalanceVersion", "int32", true, true},
    };
};

inline UserLevelStatLinkTableRow LoadUserLevelStatLinkTableRow(const nlohmann::json& object)
{
    if (!object.is_object())
    {
        throw std::runtime_error("table row must be a JSON object");
    }

    UserLevelStatLinkTableRow row{};
    const auto Level_it = object.find("Level");
    if (Level_it == object.end())
    {
        throw std::runtime_error("missing required field: UserLevelStatLinkTable.Level");
    }
    if (!Level_it->is_number_integer() && !Level_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: UserLevelStatLinkTable.Level");
    }
    row.Level = static_cast<std::int32_t>(Level_it->get<long long>());
    const auto PlayerStatTableId_it = object.find("PlayerStatTableId");
    if (PlayerStatTableId_it == object.end())
    {
        throw std::runtime_error("missing required field: UserLevelStatLinkTable.PlayerStatTableId");
    }
    if (!PlayerStatTableId_it->is_number_integer() && !PlayerStatTableId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: UserLevelStatLinkTable.PlayerStatTableId");
    }
    row.PlayerStatTableId = static_cast<std::int32_t>(PlayerStatTableId_it->get<long long>());
    const auto BalanceVersion_it = object.find("BalanceVersion");
    if (BalanceVersion_it == object.end())
    {
        throw std::runtime_error("missing required field: UserLevelStatLinkTable.BalanceVersion");
    }
    if (!BalanceVersion_it->is_number_integer() && !BalanceVersion_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: UserLevelStatLinkTable.BalanceVersion");
    }
    row.BalanceVersion = static_cast<std::int32_t>(BalanceVersion_it->get<long long>());
    return row;
}

inline std::vector<UserLevelStatLinkTableRow> LoadUserLevelStatLinkTable(const nlohmann::json& root)
{
    const nlohmann::json* rows = &root;
    if (root.is_object())
    {
        if (auto it = root.find("rows"); it != root.end())
        {
            rows = &*it;
        }
        else if (auto it = root.find("data"); it != root.end())
        {
            rows = &*it;
        }
    }

    if (!rows->is_array())
    {
        throw std::runtime_error("table root must be an array or an object with rows/data");
    }

    std::vector<UserLevelStatLinkTableRow> result;
    result.reserve(rows->size());
    for (const auto& entry : *rows)
    {
        result.push_back(LoadUserLevelStatLinkTableRow(entry));
    }
    return result;
}

inline std::vector<UserLevelStatLinkTableRow> LoadUserLevelStatLinkTableFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        throw std::runtime_error("failed to open data file");
    }

    nlohmann::json root;
    input >> root;
    return LoadUserLevelStatLinkTable(root);
}

} // namespace psh::generated::combat
