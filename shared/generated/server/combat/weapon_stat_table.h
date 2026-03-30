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
    const auto WeaponStatTableId_it = object.find("WeaponStatTableId");
    if (WeaponStatTableId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.WeaponStatTableId");
    }
    if (!WeaponStatTableId_it->is_number_integer() && !WeaponStatTableId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.WeaponStatTableId");
    }
    row.WeaponStatTableId = static_cast<std::int32_t>(WeaponStatTableId_it->get<long long>());
    const auto EnhanceLevel_it = object.find("EnhanceLevel");
    if (EnhanceLevel_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.EnhanceLevel");
    }
    if (!EnhanceLevel_it->is_number_integer() && !EnhanceLevel_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.EnhanceLevel");
    }
    row.EnhanceLevel = static_cast<std::int32_t>(EnhanceLevel_it->get<long long>());
    const auto WeaponPower_it = object.find("WeaponPower");
    if (WeaponPower_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.WeaponPower");
    }
    if (!WeaponPower_it->is_number_integer() && !WeaponPower_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.WeaponPower");
    }
    row.WeaponPower = static_cast<std::int32_t>(WeaponPower_it->get<long long>());
    const auto BaseMoveSpeed_it = object.find("BaseMoveSpeed");
    if (BaseMoveSpeed_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.BaseMoveSpeed");
    }
    if (!BaseMoveSpeed_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: WeaponStatTable.BaseMoveSpeed");
    }
    row.BaseMoveSpeed = static_cast<float>(BaseMoveSpeed_it->get<double>());
    const auto AttackSpeed_it = object.find("AttackSpeed");
    if (AttackSpeed_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.AttackSpeed");
    }
    if (!AttackSpeed_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: WeaponStatTable.AttackSpeed");
    }
    row.AttackSpeed = static_cast<float>(AttackSpeed_it->get<double>());
    const auto CastSpeed_it = object.find("CastSpeed");
    if (CastSpeed_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.CastSpeed");
    }
    if (!CastSpeed_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: WeaponStatTable.CastSpeed");
    }
    row.CastSpeed = static_cast<float>(CastSpeed_it->get<double>());
    const auto IdentityGaugeMax_it = object.find("IdentityGaugeMax");
    if (IdentityGaugeMax_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.IdentityGaugeMax");
    }
    if (!IdentityGaugeMax_it->is_number_integer() && !IdentityGaugeMax_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.IdentityGaugeMax");
    }
    row.IdentityGaugeMax = static_cast<std::int32_t>(IdentityGaugeMax_it->get<long long>());
    const auto IdentityGaugeGain_it = object.find("IdentityGaugeGain");
    if (IdentityGaugeGain_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.IdentityGaugeGain");
    }
    if (!IdentityGaugeGain_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: WeaponStatTable.IdentityGaugeGain");
    }
    row.IdentityGaugeGain = static_cast<float>(IdentityGaugeGain_it->get<double>());
    const auto ResourceType_it = object.find("ResourceType");
    if (ResourceType_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.ResourceType");
    }
    if (!ResourceType_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: WeaponStatTable.ResourceType");
    }
    EResourceType ResourceType_value{};
    if (!TryParseEResourceType(ResourceType_it->get<std::string>(), ResourceType_value))
    {
        throw std::runtime_error("failed to parse enum field: WeaponStatTable.ResourceType");
    }
    row.ResourceType = ResourceType_value;
    const auto ResourceMax_it = object.find("ResourceMax");
    if (ResourceMax_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.ResourceMax");
    }
    if (!ResourceMax_it->is_number_integer() && !ResourceMax_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.ResourceMax");
    }
    row.ResourceMax = static_cast<std::int32_t>(ResourceMax_it->get<long long>());
    const auto ResourceRegen_it = object.find("ResourceRegen");
    if (ResourceRegen_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.ResourceRegen");
    }
    if (!ResourceRegen_it->is_number_integer() && !ResourceRegen_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.ResourceRegen");
    }
    row.ResourceRegen = static_cast<std::int32_t>(ResourceRegen_it->get<long long>());
    const auto StaggerPower_it = object.find("StaggerPower");
    if (StaggerPower_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.StaggerPower");
    }
    if (!StaggerPower_it->is_number_integer() && !StaggerPower_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.StaggerPower");
    }
    row.StaggerPower = static_cast<std::int32_t>(StaggerPower_it->get<long long>());
    const auto ThreatGen_it = object.find("ThreatGen");
    if (ThreatGen_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.ThreatGen");
    }
    if (!ThreatGen_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: WeaponStatTable.ThreatGen");
    }
    row.ThreatGen = static_cast<float>(ThreatGen_it->get<double>());
    const auto ParryWindowBonus_it = object.find("ParryWindowBonus");
    if (ParryWindowBonus_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.ParryWindowBonus");
    }
    if (!ParryWindowBonus_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: WeaponStatTable.ParryWindowBonus");
    }
    row.ParryWindowBonus = static_cast<float>(ParryWindowBonus_it->get<double>());
    const auto BalanceVersion_it = object.find("BalanceVersion");
    if (BalanceVersion_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponStatTable.BalanceVersion");
    }
    if (!BalanceVersion_it->is_number_integer() && !BalanceVersion_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponStatTable.BalanceVersion");
    }
    row.BalanceVersion = static_cast<std::int32_t>(BalanceVersion_it->get<long long>());
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
        throw std::runtime_error("failed to open data file");
    }

    nlohmann::json root;
    input >> root;
    return LoadWeaponStatTable(root);
}

} // namespace psh::generated::combat
