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
    const auto WeaponId_it = object.find("WeaponId");
    if (WeaponId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.WeaponId");
    }
    if (!WeaponId_it->is_number_integer() && !WeaponId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.WeaponId");
    }
    row.WeaponId = static_cast<std::int32_t>(WeaponId_it->get<long long>());
    const auto WeaponCode_it = object.find("WeaponCode");
    if (WeaponCode_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.WeaponCode");
    }
    if (!WeaponCode_it->is_string())
    {
        throw std::runtime_error("string field must be a string: WeaponTable.WeaponCode");
    }
    row.WeaponCode = WeaponCode_it->get<std::string>();
    const auto WeaponName_it = object.find("WeaponName");
    if (WeaponName_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.WeaponName");
    }
    if (!WeaponName_it->is_string())
    {
        throw std::runtime_error("string field must be a string: WeaponTable.WeaponName");
    }
    row.WeaponName = WeaponName_it->get<std::string>();
    const auto WeaponType_it = object.find("WeaponType");
    if (WeaponType_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.WeaponType");
    }
    if (!WeaponType_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: WeaponTable.WeaponType");
    }
    EWeaponType WeaponType_value{};
    if (!TryParseEWeaponType(WeaponType_it->get<std::string>(), WeaponType_value))
    {
        throw std::runtime_error("failed to parse enum field: WeaponTable.WeaponType");
    }
    row.WeaponType = WeaponType_value;
    const auto BaseWeaponStatTableId_it = object.find("BaseWeaponStatTableId");
    if (BaseWeaponStatTableId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.BaseWeaponStatTableId");
    }
    if (!BaseWeaponStatTableId_it->is_number_integer() && !BaseWeaponStatTableId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.BaseWeaponStatTableId");
    }
    row.BaseWeaponStatTableId = static_cast<std::int32_t>(BaseWeaponStatTableId_it->get<long long>());
    const auto MaxEnhanceLevel_it = object.find("MaxEnhanceLevel");
    if (MaxEnhanceLevel_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.MaxEnhanceLevel");
    }
    if (!MaxEnhanceLevel_it->is_number_integer() && !MaxEnhanceLevel_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.MaxEnhanceLevel");
    }
    row.MaxEnhanceLevel = static_cast<std::int32_t>(MaxEnhanceLevel_it->get<long long>());
    const auto SkillSetId_it = object.find("SkillSetId");
    if (SkillSetId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.SkillSetId");
    }
    if (!SkillSetId_it->is_number_integer() && !SkillSetId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.SkillSetId");
    }
    row.SkillSetId = static_cast<std::int32_t>(SkillSetId_it->get<long long>());
    const auto IdentitySkillId_it = object.find("IdentitySkillId");
    if (IdentitySkillId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.IdentitySkillId");
    }
    if (!IdentitySkillId_it->is_number_integer() && !IdentitySkillId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.IdentitySkillId");
    }
    row.IdentitySkillId = static_cast<std::int32_t>(IdentitySkillId_it->get<long long>());
    const auto SmartDropTag_it = object.find("SmartDropTag");
    if (SmartDropTag_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.SmartDropTag");
    }
    if (!SmartDropTag_it->is_string())
    {
        throw std::runtime_error("string field must be a string: WeaponTable.SmartDropTag");
    }
    row.SmartDropTag = SmartDropTag_it->get<std::string>();
    const auto OptionPoolId_it = object.find("OptionPoolId");
    if (OptionPoolId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.OptionPoolId");
    }
    if (!OptionPoolId_it->is_number_integer() && !OptionPoolId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.OptionPoolId");
    }
    row.OptionPoolId = static_cast<std::int32_t>(OptionPoolId_it->get<long long>());
    const auto BaseDurabilityMax_it = object.find("BaseDurabilityMax");
    if (BaseDurabilityMax_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.BaseDurabilityMax");
    }
    if (!BaseDurabilityMax_it->is_number_integer() && !BaseDurabilityMax_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.BaseDurabilityMax");
    }
    row.BaseDurabilityMax = static_cast<std::int32_t>(BaseDurabilityMax_it->get<long long>());
    const auto RepairCostRate_it = object.find("RepairCostRate");
    if (RepairCostRate_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.RepairCostRate");
    }
    if (!RepairCostRate_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: WeaponTable.RepairCostRate");
    }
    row.RepairCostRate = static_cast<float>(RepairCostRate_it->get<double>());
    const auto EquipLevelMin_it = object.find("EquipLevelMin");
    if (EquipLevelMin_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.EquipLevelMin");
    }
    if (!EquipLevelMin_it->is_number_integer() && !EquipLevelMin_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.EquipLevelMin");
    }
    row.EquipLevelMin = static_cast<std::int32_t>(EquipLevelMin_it->get<long long>());
    const auto TradeLimitCount_it = object.find("TradeLimitCount");
    if (TradeLimitCount_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.TradeLimitCount");
    }
    if (!TradeLimitCount_it->is_number_integer() && !TradeLimitCount_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.TradeLimitCount");
    }
    row.TradeLimitCount = static_cast<std::int32_t>(TradeLimitCount_it->get<long long>());
    const auto AnimationSetId_it = object.find("AnimationSetId");
    if (AnimationSetId_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.AnimationSetId");
    }
    if (!AnimationSetId_it->is_number_integer() && !AnimationSetId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.AnimationSetId");
    }
    row.AnimationSetId = static_cast<std::int32_t>(AnimationSetId_it->get<long long>());
    const auto IconKey_it = object.find("IconKey");
    if (IconKey_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.IconKey");
    }
    if (!IconKey_it->is_string())
    {
        throw std::runtime_error("string field must be a string: WeaponTable.IconKey");
    }
    row.IconKey = IconKey_it->get<std::string>();
    const auto ModelKey_it = object.find("ModelKey");
    if (ModelKey_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.ModelKey");
    }
    if (!ModelKey_it->is_string())
    {
        throw std::runtime_error("string field must be a string: WeaponTable.ModelKey");
    }
    row.ModelKey = ModelKey_it->get<std::string>();
    const auto DisplayOrder_it = object.find("DisplayOrder");
    if (DisplayOrder_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.DisplayOrder");
    }
    if (!DisplayOrder_it->is_number_integer() && !DisplayOrder_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.DisplayOrder");
    }
    row.DisplayOrder = static_cast<std::int32_t>(DisplayOrder_it->get<long long>());
    const auto HandType_it = object.find("HandType");
    if (HandType_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.HandType");
    }
    if (!HandType_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: WeaponTable.HandType");
    }
    EWeaponHandType HandType_value{};
    if (!TryParseEWeaponHandType(HandType_it->get<std::string>(), HandType_value))
    {
        throw std::runtime_error("failed to parse enum field: WeaponTable.HandType");
    }
    row.HandType = HandType_value;
    const auto CombatRoleTag_it = object.find("CombatRoleTag");
    if (CombatRoleTag_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.CombatRoleTag");
    }
    if (!CombatRoleTag_it->is_string())
    {
        throw std::runtime_error("string field must be a string: WeaponTable.CombatRoleTag");
    }
    row.CombatRoleTag = CombatRoleTag_it->get<std::string>();
    const auto RangeProfile_it = object.find("RangeProfile");
    if (RangeProfile_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.RangeProfile");
    }
    if (!RangeProfile_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: WeaponTable.RangeProfile");
    }
    ERangeProfile RangeProfile_value{};
    if (!TryParseERangeProfile(RangeProfile_it->get<std::string>(), RangeProfile_value))
    {
        throw std::runtime_error("failed to parse enum field: WeaponTable.RangeProfile");
    }
    row.RangeProfile = RangeProfile_value;
    const auto BalanceVersion_it = object.find("BalanceVersion");
    if (BalanceVersion_it == object.end())
    {
        throw std::runtime_error("missing required field: WeaponTable.BalanceVersion");
    }
    if (!BalanceVersion_it->is_number_integer() && !BalanceVersion_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: WeaponTable.BalanceVersion");
    }
    row.BalanceVersion = static_cast<std::int32_t>(BalanceVersion_it->get<long long>());
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
        throw std::runtime_error("failed to open data file");
    }

    nlohmann::json root;
    input >> root;
    return LoadWeaponTable(root);
}

} // namespace psh::generated::combat
