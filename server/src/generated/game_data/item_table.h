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

struct ItemTableRow
{
    std::int32_t ItemId{};
    std::string ItemCode{};
    std::string ItemName{};
    EItemCategory ItemCategory{};
    EItemRarity Rarity{};
    EEquipSlot EquipSlot{};
    EWeaponType WeaponType{};
    std::int32_t MaxStackCount{};
    std::int32_t MaxDurability{};
    std::int32_t MaxTradeCount{};
    std::int32_t SellValue{};
    bool bCanUseSharedStorage{};
    bool bCanBindToConsumableQuickSlot{};
    std::string IconKey{};
    std::int32_t BalanceVersion{};
};

struct ItemTableSchema
{
    static constexpr std::string_view TableName = "ItemTable";
    static constexpr std::string_view SchemaFile = "item_table.schema.json";
    static constexpr std::string_view DataFile = "item_table.data.json";
    static constexpr std::size_t RowCount = 5;
    static constexpr std::size_t FieldCount = 15;
    static constexpr std::array<TableFieldMetadata, FieldCount> Fields = {
        TableFieldMetadata{"ItemId", "int32", true, false},
        TableFieldMetadata{"ItemCode", "string", true, false},
        TableFieldMetadata{"ItemName", "string", true, false},
        TableFieldMetadata{"ItemCategory", "enum", true, false},
        TableFieldMetadata{"Rarity", "enum", true, false},
        TableFieldMetadata{"EquipSlot", "enum", true, false},
        TableFieldMetadata{"WeaponType", "enum", true, false},
        TableFieldMetadata{"MaxStackCount", "int32", true, false},
        TableFieldMetadata{"MaxDurability", "int32", true, false},
        TableFieldMetadata{"MaxTradeCount", "int32", true, false},
        TableFieldMetadata{"SellValue", "int32", true, false},
        TableFieldMetadata{"bCanUseSharedStorage", "bool", true, false},
        TableFieldMetadata{"bCanBindToConsumableQuickSlot", "bool", true, false},
        TableFieldMetadata{"IconKey", "string", true, false},
        TableFieldMetadata{"BalanceVersion", "int32", true, false},
    };
};

inline ItemTableRow LoadItemTableRow(const nlohmann::json& object)
{
    if (!object.is_object())
    {
        throw std::runtime_error("table row must be a JSON object");
    }

    ItemTableRow row{};
    if (const auto it = object.find("ItemId"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.ItemId = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.ItemId");
    }
    if (const auto it = object.find("ItemCode"); it != object.end() && it->is_string())
    {
        row.ItemCode = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.ItemCode");
    }
    if (const auto it = object.find("ItemName"); it != object.end() && it->is_string())
    {
        row.ItemName = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.ItemName");
    }
    if (const auto it = object.find("ItemCategory"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EItemCategory>(it->get<std::string_view>()))
        {
            row.ItemCategory = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: ItemTable.ItemCategory");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.ItemCategory");
    }
    if (const auto it = object.find("Rarity"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EItemRarity>(it->get<std::string_view>()))
        {
            row.Rarity = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: ItemTable.Rarity");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.Rarity");
    }
    if (const auto it = object.find("EquipSlot"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EEquipSlot>(it->get<std::string_view>()))
        {
            row.EquipSlot = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: ItemTable.EquipSlot");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.EquipSlot");
    }
    if (const auto it = object.find("WeaponType"); it != object.end() && it->is_string())
    {
        if (auto parsed = magic_enum::enum_cast<EWeaponType>(it->get<std::string_view>()))
        {
            row.WeaponType = *parsed;
        }
        else
        {
            throw std::runtime_error("failed to parse enum: ItemTable.WeaponType");
        }
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.WeaponType");
    }
    if (const auto it = object.find("MaxStackCount"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.MaxStackCount = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.MaxStackCount");
    }
    if (const auto it = object.find("MaxDurability"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.MaxDurability = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.MaxDurability");
    }
    if (const auto it = object.find("MaxTradeCount"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.MaxTradeCount = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.MaxTradeCount");
    }
    if (const auto it = object.find("SellValue"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.SellValue = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.SellValue");
    }
    if (const auto it = object.find("bCanUseSharedStorage"); it != object.end() && it->is_boolean())
    {
        row.bCanUseSharedStorage = it->get<bool>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.bCanUseSharedStorage");
    }
    if (const auto it = object.find("bCanBindToConsumableQuickSlot"); it != object.end() && it->is_boolean())
    {
        row.bCanBindToConsumableQuickSlot = it->get<bool>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.bCanBindToConsumableQuickSlot");
    }
    if (const auto it = object.find("IconKey"); it != object.end() && it->is_string())
    {
        row.IconKey = it->get<std::string>();
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.IconKey");
    }
    if (const auto it = object.find("BalanceVersion"); it != object.end() && (it->is_number_integer() || it->is_number_unsigned()))
    {
        row.BalanceVersion = static_cast<std::int32_t>(it->get<long long>());
    }
    else
    {
        throw std::runtime_error("missing or invalid field: ItemTable.BalanceVersion");
    }
    return row;
}

inline std::vector<ItemTableRow> LoadItemTable(const nlohmann::json& root)
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

    std::vector<ItemTableRow> result;
    result.reserve(rows->size());
    for (const auto& entry : *rows)
    {
        result.push_back(LoadItemTableRow(entry));
    }
    return result;
}

inline std::vector<ItemTableRow> LoadItemTableFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        throw std::runtime_error("failed to open data file: " + path.string());
    }

    nlohmann::json root;
    input >> root;
    return LoadItemTable(root);
}

} // namespace psh::generated