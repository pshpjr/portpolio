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
    if (const auto it = object.find("PlayerStatTableId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.PlayerStatTableId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.PlayerStatTableId");
    }
    if (const auto it = object.find("Level"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.Level = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.Level");
    }
    if (const auto it = object.find("MaxHP"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.MaxHP = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.MaxHP");
    }
    if (const auto it = object.find("AttackPower"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.AttackPower = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.AttackPower");
    }
    if (const auto it = object.find("Defense"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.Defense = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.Defense");
    }
    if (const auto it = object.find("CriticalChance"); it != object.end() && it->is_number())
    {
        row.CriticalChance = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.CriticalChance");
    }
    if (const auto it = object.find("CriticalDamage"); it != object.end() && it->is_number())
    {
        row.CriticalDamage = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.CriticalDamage");
    }
    if (const auto it = object.find("CooldownReduction"); it != object.end() && it->is_number())
    {
        row.CooldownReduction = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.CooldownReduction");
    }
    if (const auto it = object.find("DamageReduction"); it != object.end() && it->is_number())
    {
        row.DamageReduction = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.DamageReduction");
    }
    if (const auto it = object.find("MoveSpeedBonus"); it != object.end() && it->is_number())
    {
        row.MoveSpeedBonus = static_cast<float>(it->get<double>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.MoveSpeedBonus");
    }
    if (const auto it = object.find("BalanceVersion"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BalanceVersion = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: PlayerStatTable.BalanceVersion");
    }
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
        throw std::runtime_error("failed to open data file: " + path.string());
    }

    nlohmann::json root;
    input >> root;
    return LoadPlayerStatTable(root);
}

} // namespace psh::generated