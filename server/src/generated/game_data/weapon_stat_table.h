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

struct WeaponStatTableRow
{
    std::int32_t WeaponStatTableId{};
    std::int32_t EnhanceLevel{};
    std::int32_t WeaponPower{};
    float BaseMoveSpeed{};
    float AttackSpeed{};
    float CastSpeed{};
    std::int32_t IdentityGaugeMax{};
    float IdentityGaugeGain{};
    EResourceType ResourceType{};
    std::int32_t ResourceMax{};
    std::int32_t ResourceRegen{};
    std::int32_t StaggerPower{};
    float ThreatGen{};
    float ParryWindowBonus{};
    std::int32_t BalanceVersion{};
};

struct WeaponStatTableSchema
{
    static constexpr std::string_view TableName = "WeaponStatTable";
    static constexpr std::string_view SchemaFile = "weapon_stat_table.schema.json";
    static constexpr std::string_view DataFile = "weapon_stat_table.data.json";
    static constexpr std::size_t RowCount = 75;
    static constexpr std::size_t FieldCount = 15;
    static constexpr std::array<TableFieldMetadata, FieldCount> Fields = {
        TableFieldMetadata{"WeaponStatTableId", "int32", true, true},
        TableFieldMetadata{"EnhanceLevel", "int32", true, false},
        TableFieldMetadata{"WeaponPower", "int32", true, false},
        TableFieldMetadata{"BaseMoveSpeed", "float", true, false},
        TableFieldMetadata{"AttackSpeed", "float", true, false},
        TableFieldMetadata{"CastSpeed", "float", true, false},
        TableFieldMetadata{"IdentityGaugeMax", "int32", true, false},
        TableFieldMetadata{"IdentityGaugeGain", "float", true, false},
        TableFieldMetadata{"ResourceType", "enum", true, false},
        TableFieldMetadata{"ResourceMax", "int32", true, false},
        TableFieldMetadata{"ResourceRegen", "int32", true, false},
        TableFieldMetadata{"StaggerPower", "int32", true, false},
        TableFieldMetadata{"ThreatGen", "float", true, false},
        TableFieldMetadata{"ParryWindowBonus", "float", true, false},
        TableFieldMetadata{"BalanceVersion", "int32", true, false},
    };
};

inline WeaponStatTableRow LoadWeaponStatTableRow(const nlohmann::json& object)
{
    if (!object.is_object())
    {
        throw std::runtime_error("table row must be a JSON object");
    }

    WeaponStatTableRow row{};
    if (const auto it = object.find("WeaponStatTableId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.WeaponStatTableId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.WeaponStatTableId");
    }
    if (const auto it = object.find("EnhanceLevel"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.EnhanceLevel = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.EnhanceLevel");
    }
    if (const auto it = object.find("WeaponPower"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.WeaponPower = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.WeaponPower");
    }
    if (const auto it = object.find("BaseMoveSpeed"); it != object.end() && it->is_number())
    {
        row.BaseMoveSpeed = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.BaseMoveSpeed");
    }
    if (const auto it = object.find("AttackSpeed"); it != object.end() && it->is_number())
    {
        row.AttackSpeed = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.AttackSpeed");
    }
    if (const auto it = object.find("CastSpeed"); it != object.end() && it->is_number())
    {
        row.CastSpeed = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.CastSpeed");
    }
    if (const auto it = object.find("IdentityGaugeMax"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.IdentityGaugeMax = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.IdentityGaugeMax");
    }
    if (const auto it = object.find("IdentityGaugeGain"); it != object.end() && it->is_number())
    {
        row.IdentityGaugeGain = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.IdentityGaugeGain");
    }
    if (const auto it = object.find("ResourceType"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EResourceType>(it->get<std::string_view>()))
        {
            row.ResourceType = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: WeaponStatTable.ResourceType");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.ResourceType");
    }
    if (const auto it = object.find("ResourceMax"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.ResourceMax = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.ResourceMax");
    }
    if (const auto it = object.find("ResourceRegen"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.ResourceRegen = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.ResourceRegen");
    }
    if (const auto it = object.find("StaggerPower"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.StaggerPower = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.StaggerPower");
    }
    if (const auto it = object.find("ThreatGen"); it != object.end() && it->is_number())
    {
        row.ThreatGen = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.ThreatGen");
    }
    if (const auto it = object.find("ParryWindowBonus"); it != object.end() && it->is_number())
    {
        row.ParryWindowBonus = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.ParryWindowBonus");
    }
    if (const auto it = object.find("BalanceVersion"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BalanceVersion = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponStatTable.BalanceVersion");
    }
    return row;
}

inline std::vector<WeaponStatTableRow> LoadWeaponStatTable(const nlohmann::json& root)
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

    std::vector<WeaponStatTableRow> result;
    result.reserve(rows->size());
    for (const auto& entry : *rows)
    {
        result.push_back(LoadWeaponStatTableRow(entry));
    }
    return result;
}

inline std::vector<WeaponStatTableRow> LoadWeaponStatTableFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        throw std::runtime_error("failed to open data file: " + path.string());
    }

    nlohmann::json root;
    input >> root;
    return LoadWeaponStatTable(root);
}

} // namespace psh::generated