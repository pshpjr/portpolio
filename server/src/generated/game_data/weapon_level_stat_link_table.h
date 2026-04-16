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

struct WeaponLevelStatLinkTableRow
{
    EWeaponType WeaponType{};
    std::int32_t EnhanceLevel{};
    std::int32_t WeaponStatTableId{};
    std::int32_t BalanceVersion{};
};

struct WeaponLevelStatLinkTableSchema
{
    static constexpr std::string_view TableName = "WeaponLevelStatLinkTable";
    static constexpr std::string_view SchemaFile = "weapon_level_stat_link_table.schema.json";
    static constexpr std::string_view DataFile = "weapon_level_stat_link_table.data.json";
    static constexpr std::size_t RowCount = 75;
    static constexpr std::size_t FieldCount = 4;
    static constexpr std::array<TableFieldMetadata, FieldCount> Fields = {
        TableFieldMetadata{"WeaponType", "enum", true, true},
        TableFieldMetadata{"EnhanceLevel", "int32", true, true},
        TableFieldMetadata{"WeaponStatTableId", "int32", true, false},
        TableFieldMetadata{"BalanceVersion", "int32", true, true},
    };
};

inline WeaponLevelStatLinkTableRow LoadWeaponLevelStatLinkTableRow(const nlohmann::json& object)
{
    if (!object.is_object())
    {
        throw std::runtime_error("table row must be a JSON object");
    }

    WeaponLevelStatLinkTableRow row{};
    if (const auto it = object.find("WeaponType"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EWeaponType>(it->get<std::string_view>()))
        {
            row.WeaponType = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: WeaponLevelStatLinkTable.WeaponType");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponLevelStatLinkTable.WeaponType");
    }
    if (const auto it = object.find("EnhanceLevel"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.EnhanceLevel = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponLevelStatLinkTable.EnhanceLevel");
    }
    if (const auto it = object.find("WeaponStatTableId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.WeaponStatTableId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponLevelStatLinkTable.WeaponStatTableId");
    }
    if (const auto it = object.find("BalanceVersion"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BalanceVersion = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponLevelStatLinkTable.BalanceVersion");
    }
    return row;
}

inline std::vector<WeaponLevelStatLinkTableRow> LoadWeaponLevelStatLinkTable(const nlohmann::json& root)
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

    std::vector<WeaponLevelStatLinkTableRow> result;
    result.reserve(rows->size());
    for (const auto& entry : *rows)
    {
        result.push_back(LoadWeaponLevelStatLinkTableRow(entry));
    }
    return result;
}

inline std::vector<WeaponLevelStatLinkTableRow> LoadWeaponLevelStatLinkTableFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        throw std::runtime_error("failed to open data file: " + path.string());
    }

    nlohmann::json root;
    input >> root;
    return LoadWeaponLevelStatLinkTable(root);
}

} // namespace psh::generated