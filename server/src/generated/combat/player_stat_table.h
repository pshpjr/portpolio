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

struct PlayerStatTableRow
{
    std::int32_t PlayerStatTableId{};
    std::int32_t Level{};
    std::int32_t MaxHP{};
    std::int32_t AttackPower{};
    std::int32_t Defense{};
    float CriticalChance{};
    float CriticalDamage{};
    float CooldownReduction{};
    float DamageReduction{};
    float MoveSpeedBonus{};
    std::int32_t BalanceVersion{};
};

struct PlayerStatTableSchema
{
    static constexpr std::string_view TableName = "PlayerStatTable";
    static constexpr std::string_view SchemaFile = "player_stat_table.schema.json";
    static constexpr std::string_view DataFile = "player_stat_table.data.json";
    static constexpr std::size_t RowCount = 40;
    static constexpr std::size_t FieldCount = 11;
    static constexpr std::array<TableFieldMetadata, FieldCount> Fields = {
        TableFieldMetadata{"PlayerStatTableId", "int32", true, true},
        TableFieldMetadata{"Level", "int32", true, false},
        TableFieldMetadata{"MaxHP", "int32", true, false},
        TableFieldMetadata{"AttackPower", "int32", true, false},
        TableFieldMetadata{"Defense", "int32", true, false},
        TableFieldMetadata{"CriticalChance", "float", true, false},
        TableFieldMetadata{"CriticalDamage", "float", true, false},
        TableFieldMetadata{"CooldownReduction", "float", true, false},
        TableFieldMetadata{"DamageReduction", "float", true, false},
        TableFieldMetadata{"MoveSpeedBonus", "float", true, false},
        TableFieldMetadata{"BalanceVersion", "int32", true, false},
    };
};

inline PlayerStatTableRow LoadPlayerStatTableRow(const nlohmann::json& object)
{
    if (!object.is_object())
    {
        throw std::runtime_error("table row must be a JSON object");
    }

    PlayerStatTableRow row{};
    const auto PlayerStatTableId_it = object.find("PlayerStatTableId");
    if (PlayerStatTableId_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.PlayerStatTableId");
    }
    if (!PlayerStatTableId_it->is_number_integer() && !PlayerStatTableId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: PlayerStatTable.PlayerStatTableId");
    }
    row.PlayerStatTableId = static_cast<std::int32_t>(PlayerStatTableId_it->get<long long>());
    const auto Level_it = object.find("Level");
    if (Level_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.Level");
    }
    if (!Level_it->is_number_integer() && !Level_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: PlayerStatTable.Level");
    }
    row.Level = static_cast<std::int32_t>(Level_it->get<long long>());
    const auto MaxHP_it = object.find("MaxHP");
    if (MaxHP_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.MaxHP");
    }
    if (!MaxHP_it->is_number_integer() && !MaxHP_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: PlayerStatTable.MaxHP");
    }
    row.MaxHP = static_cast<std::int32_t>(MaxHP_it->get<long long>());
    const auto AttackPower_it = object.find("AttackPower");
    if (AttackPower_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.AttackPower");
    }
    if (!AttackPower_it->is_number_integer() && !AttackPower_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: PlayerStatTable.AttackPower");
    }
    row.AttackPower = static_cast<std::int32_t>(AttackPower_it->get<long long>());
    const auto Defense_it = object.find("Defense");
    if (Defense_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.Defense");
    }
    if (!Defense_it->is_number_integer() && !Defense_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: PlayerStatTable.Defense");
    }
    row.Defense = static_cast<std::int32_t>(Defense_it->get<long long>());
    const auto CriticalChance_it = object.find("CriticalChance");
    if (CriticalChance_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.CriticalChance");
    }
    if (!CriticalChance_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: PlayerStatTable.CriticalChance");
    }
    row.CriticalChance = static_cast<float>(CriticalChance_it->get<double>());
    const auto CriticalDamage_it = object.find("CriticalDamage");
    if (CriticalDamage_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.CriticalDamage");
    }
    if (!CriticalDamage_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: PlayerStatTable.CriticalDamage");
    }
    row.CriticalDamage = static_cast<float>(CriticalDamage_it->get<double>());
    const auto CooldownReduction_it = object.find("CooldownReduction");
    if (CooldownReduction_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.CooldownReduction");
    }
    if (!CooldownReduction_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: PlayerStatTable.CooldownReduction");
    }
    row.CooldownReduction = static_cast<float>(CooldownReduction_it->get<double>());
    const auto DamageReduction_it = object.find("DamageReduction");
    if (DamageReduction_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.DamageReduction");
    }
    if (!DamageReduction_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: PlayerStatTable.DamageReduction");
    }
    row.DamageReduction = static_cast<float>(DamageReduction_it->get<double>());
    const auto MoveSpeedBonus_it = object.find("MoveSpeedBonus");
    if (MoveSpeedBonus_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.MoveSpeedBonus");
    }
    if (!MoveSpeedBonus_it->is_number())
    {
        throw std::runtime_error("float field must be numeric: PlayerStatTable.MoveSpeedBonus");
    }
    row.MoveSpeedBonus = static_cast<float>(MoveSpeedBonus_it->get<double>());
    const auto BalanceVersion_it = object.find("BalanceVersion");
    if (BalanceVersion_it == object.end())
    {
        throw std::runtime_error("missing required field: PlayerStatTable.BalanceVersion");
    }
    if (!BalanceVersion_it->is_number_integer() && !BalanceVersion_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: PlayerStatTable.BalanceVersion");
    }
    row.BalanceVersion = static_cast<std::int32_t>(BalanceVersion_it->get<long long>());
    return row;
}

inline std::vector<PlayerStatTableRow> LoadPlayerStatTable(const nlohmann::json& root)
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

    std::vector<PlayerStatTableRow> result;
    result.reserve(rows->size());
    for (const auto& entry : *rows)
    {
        result.push_back(LoadPlayerStatTableRow(entry));
    }
    return result;
}

inline std::vector<PlayerStatTableRow> LoadPlayerStatTableFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        throw std::runtime_error("failed to open data file");
    }

    nlohmann::json root;
    input >> root;
    return LoadPlayerStatTable(root);
}

} // namespace psh::generated::combat
