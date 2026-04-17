// Auto-generated. Do not edit.
#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "table_common.h"
#include "enums.h"

namespace psh::generated {

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
    if (const auto it = object.find("Level"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.Level = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: UserLevelStatLinkTable.Level");
    }
    if (const auto it = object.find("PlayerStatTableId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.PlayerStatTableId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: UserLevelStatLinkTable.PlayerStatTableId");
    }
    if (const auto it = object.find("BalanceVersion"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BalanceVersion = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: UserLevelStatLinkTable.BalanceVersion");
    }
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
        throw std::runtime_error("failed to open data file: " + path.string());
    }

    nlohmann::json root;
    input >> root;
    return LoadUserLevelStatLinkTable(root);
}

} // namespace psh::generated