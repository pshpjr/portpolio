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

struct WeaponTableRow
{
    std::int32_t WeaponId{};
    std::string WeaponCode{};
    std::string WeaponName{};
    EWeaponType WeaponType{};
    std::int32_t BaseWeaponStatTableId{};
    std::int32_t MaxEnhanceLevel{};
    std::int32_t SkillSetId{};
    std::int32_t IdentitySkillId{};
    std::string SmartDropTag{};
    std::int32_t OptionPoolId{};
    std::int32_t BaseDurabilityMax{};
    float RepairCostRate{};
    std::int32_t EquipLevelMin{};
    std::int32_t TradeLimitCount{};
    std::int32_t AnimationSetId{};
    std::string IconKey{};
    std::string ModelKey{};
    std::int32_t DisplayOrder{};
    EWeaponHandType HandType{};
    std::string CombatRoleTag{};
    ERangeProfile RangeProfile{};
    std::int32_t BalanceVersion{};
};

struct WeaponTableSchema
{
    static constexpr std::string_view TableName = "WeaponTable";
    static constexpr std::string_view SchemaFile = "weapon_table.schema.json";
    static constexpr std::string_view DataFile = "weapon_table.data.json";
    static constexpr std::size_t RowCount = 3;
    static constexpr std::size_t FieldCount = 22;
    static constexpr std::array<TableFieldMetadata, FieldCount> Fields = {
        TableFieldMetadata{"WeaponId", "int32", true, true},
        TableFieldMetadata{"WeaponCode", "string", true, false},
        TableFieldMetadata{"WeaponName", "string", true, false},
        TableFieldMetadata{"WeaponType", "enum", true, false},
        TableFieldMetadata{"BaseWeaponStatTableId", "int32", true, false},
        TableFieldMetadata{"MaxEnhanceLevel", "int32", true, false},
        TableFieldMetadata{"SkillSetId", "int32", true, false},
        TableFieldMetadata{"IdentitySkillId", "int32", true, false},
        TableFieldMetadata{"SmartDropTag", "string", true, false},
        TableFieldMetadata{"OptionPoolId", "int32", true, false},
        TableFieldMetadata{"BaseDurabilityMax", "int32", true, false},
        TableFieldMetadata{"RepairCostRate", "float", true, false},
        TableFieldMetadata{"EquipLevelMin", "int32", true, false},
        TableFieldMetadata{"TradeLimitCount", "int32", true, false},
        TableFieldMetadata{"AnimationSetId", "int32", true, false},
        TableFieldMetadata{"IconKey", "string", true, false},
        TableFieldMetadata{"ModelKey", "string", true, false},
        TableFieldMetadata{"DisplayOrder", "int32", true, false},
        TableFieldMetadata{"HandType", "enum", true, false},
        TableFieldMetadata{"CombatRoleTag", "string", true, false},
        TableFieldMetadata{"RangeProfile", "enum", true, false},
        TableFieldMetadata{"BalanceVersion", "int32", true, false},
    };
};

inline WeaponTableRow LoadWeaponTableRow(const nlohmann::json& object)
{
    if (!object.is_object())
    {
        throw std::runtime_error("table row must be a JSON object");
    }

    WeaponTableRow row{};
    if (const auto it = object.find("WeaponId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.WeaponId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.WeaponId");
    }
    if (const auto it = object.find("WeaponCode"); it != object.end() && it->is_string())
    {
        row.WeaponCode = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.WeaponCode");
    }
    if (const auto it = object.find("WeaponName"); it != object.end() && it->is_string())
    {
        row.WeaponName = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.WeaponName");
    }
    if (const auto it = object.find("WeaponType"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EWeaponType>(it->get<std::string_view>()))
        {
            row.WeaponType = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: WeaponTable.WeaponType");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.WeaponType");
    }
    if (const auto it = object.find("BaseWeaponStatTableId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BaseWeaponStatTableId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.BaseWeaponStatTableId");
    }
    if (const auto it = object.find("MaxEnhanceLevel"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.MaxEnhanceLevel = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.MaxEnhanceLevel");
    }
    if (const auto it = object.find("SkillSetId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.SkillSetId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.SkillSetId");
    }
    if (const auto it = object.find("IdentitySkillId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.IdentitySkillId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.IdentitySkillId");
    }
    if (const auto it = object.find("SmartDropTag"); it != object.end() && it->is_string())
    {
        row.SmartDropTag = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.SmartDropTag");
    }
    if (const auto it = object.find("OptionPoolId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.OptionPoolId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.OptionPoolId");
    }
    if (const auto it = object.find("BaseDurabilityMax"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BaseDurabilityMax = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.BaseDurabilityMax");
    }
    if (const auto it = object.find("RepairCostRate"); it != object.end() && it->is_number())
    {
        row.RepairCostRate = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.RepairCostRate");
    }
    if (const auto it = object.find("EquipLevelMin"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.EquipLevelMin = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.EquipLevelMin");
    }
    if (const auto it = object.find("TradeLimitCount"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.TradeLimitCount = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.TradeLimitCount");
    }
    if (const auto it = object.find("AnimationSetId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.AnimationSetId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.AnimationSetId");
    }
    if (const auto it = object.find("IconKey"); it != object.end() && it->is_string())
    {
        row.IconKey = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.IconKey");
    }
    if (const auto it = object.find("ModelKey"); it != object.end() && it->is_string())
    {
        row.ModelKey = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.ModelKey");
    }
    if (const auto it = object.find("DisplayOrder"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.DisplayOrder = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.DisplayOrder");
    }
    if (const auto it = object.find("HandType"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EWeaponHandType>(it->get<std::string_view>()))
        {
            row.HandType = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: WeaponTable.HandType");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.HandType");
    }
    if (const auto it = object.find("CombatRoleTag"); it != object.end() && it->is_string())
    {
        row.CombatRoleTag = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.CombatRoleTag");
    }
    if (const auto it = object.find("RangeProfile"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<ERangeProfile>(it->get<std::string_view>()))
        {
            row.RangeProfile = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: WeaponTable.RangeProfile");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.RangeProfile");
    }
    if (const auto it = object.find("BalanceVersion"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BalanceVersion = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: WeaponTable.BalanceVersion");
    }
    return row;
}

inline std::vector<WeaponTableRow> LoadWeaponTable(const nlohmann::json& root)
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

    std::vector<WeaponTableRow> result;
    result.reserve(rows->size());
    for (const auto& entry : *rows)
    {
        result.push_back(LoadWeaponTableRow(entry));
    }
    return result;
}

inline std::vector<WeaponTableRow> LoadWeaponTableFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        throw std::runtime_error("failed to open data file: " + path.string());
    }

    nlohmann::json root;
    input >> root;
    return LoadWeaponTable(root);
}

} // namespace psh::generated