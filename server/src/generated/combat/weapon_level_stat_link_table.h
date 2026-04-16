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

#include "../combat_data_common.h"
#include "../combat_data_enums.h"

namespace psh::generated::combat {

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
    const auto WeaponType_it = object.find("WeaponType");
    if (WeaponType_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponLevelStatLinkTable.WeaponType");
    }
    if (!WeaponType_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: WeaponLevelStatLinkTable.WeaponType");
    }
    EWeaponType WeaponType_value{};
    if (!TryParseEWeaponType(WeaponType_it->get<std::string>(), WeaponType_value))
    {
        throw std::runtime_error("failed to parse enum field: WeaponLevelStatLinkTable.WeaponType");
    }
    row.WeaponType = WeaponType_value;
    const auto EnhanceLevel_it = object.find("EnhanceLevel");
    if (EnhanceLevel_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponLevelStatLinkTable.EnhanceLevel");
    }
    if (!EnhanceLevel_it->is_number_integer() && !EnhanceLevel_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponLevelStatLinkTable.EnhanceLevel");
    }
    row.EnhanceLevel = static_cast<std::int32_t>(EnhanceLevel_it->get<long long>());
    const auto WeaponStatTableId_it = object.find("WeaponStatTableId");
    if (WeaponStatTableId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponLevelStatLinkTable.WeaponStatTableId");
    }
    if (!WeaponStatTableId_it->is_number_integer() && !WeaponStatTableId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponLevelStatLinkTable.WeaponStatTableId");
    }
    row.WeaponStatTableId = static_cast<std::int32_t>(WeaponStatTableId_it->get<long long>());
    const auto BalanceVersion_it = object.find("BalanceVersion");
    if (BalanceVersion_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponLevelStatLinkTable.BalanceVersion");
    }
    if (!BalanceVersion_it->is_number_integer() && !BalanceVersion_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponLevelStatLinkTable.BalanceVersion");
    }
    row.BalanceVersion = static_cast<std::int32_t>(BalanceVersion_it->get<long long>());
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
        throw std::runtime_error("failed to open data file");
    }

    nlohmann::json root;
    input >> root;
    return LoadWeaponLevelStatLinkTable(root);
}

} // namespace psh::generated::combat
