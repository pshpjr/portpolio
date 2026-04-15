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
    const auto ItemId_it = object.find("ItemId");
    if (ItemId_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.ItemId");
    }
    if (!ItemId_it->is_number_integer() && !ItemId_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: ItemTable.ItemId");
    }
    row.ItemId = static_cast<std::int32_t>(ItemId_it->get<long long>());
    const auto ItemCode_it = object.find("ItemCode");
    if (ItemCode_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.ItemCode");
    }
    if (!ItemCode_it->is_string())
    {
        throw std::runtime_error("string field must be a string: ItemTable.ItemCode");
    }
    row.ItemCode = ItemCode_it->get<std::string>();
    const auto ItemName_it = object.find("ItemName");
    if (ItemName_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.ItemName");
    }
    if (!ItemName_it->is_string())
    {
        throw std::runtime_error("string field must be a string: ItemTable.ItemName");
    }
    row.ItemName = ItemName_it->get<std::string>();
    const auto ItemCategory_it = object.find("ItemCategory");
    if (ItemCategory_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.ItemCategory");
    }
    if (!ItemCategory_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: ItemTable.ItemCategory");
    }
    EItemCategory ItemCategory_value{};
    if (!TryParseEItemCategory(ItemCategory_it->get<std::string>(), ItemCategory_value))
    {
        throw std::runtime_error("failed to parse enum field: ItemTable.ItemCategory");
    }
    row.ItemCategory = ItemCategory_value;
    const auto Rarity_it = object.find("Rarity");
    if (Rarity_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.Rarity");
    }
    if (!Rarity_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: ItemTable.Rarity");
    }
    EItemRarity Rarity_value{};
    if (!TryParseEItemRarity(Rarity_it->get<std::string>(), Rarity_value))
    {
        throw std::runtime_error("failed to parse enum field: ItemTable.Rarity");
    }
    row.Rarity = Rarity_value;
    const auto EquipSlot_it = object.find("EquipSlot");
    if (EquipSlot_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.EquipSlot");
    }
    if (!EquipSlot_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: ItemTable.EquipSlot");
    }
    EEquipSlot EquipSlot_value{};
    if (!TryParseEEquipSlot(EquipSlot_it->get<std::string>(), EquipSlot_value))
    {
        throw std::runtime_error("failed to parse enum field: ItemTable.EquipSlot");
    }
    row.EquipSlot = EquipSlot_value;
    const auto WeaponType_it = object.find("WeaponType");
    if (WeaponType_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.WeaponType");
    }
    if (!WeaponType_it->is_string())
    {
        throw std::runtime_error("enum fields must be strings: ItemTable.WeaponType");
    }
    EWeaponType WeaponType_value{};
    if (!TryParseEWeaponType(WeaponType_it->get<std::string>(), WeaponType_value))
    {
        throw std::runtime_error("failed to parse enum field: ItemTable.WeaponType");
    }
    row.WeaponType = WeaponType_value;
    const auto MaxStackCount_it = object.find("MaxStackCount");
    if (MaxStackCount_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.MaxStackCount");
    }
    if (!MaxStackCount_it->is_number_integer() && !MaxStackCount_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: ItemTable.MaxStackCount");
    }
    row.MaxStackCount = static_cast<std::int32_t>(MaxStackCount_it->get<long long>());
    const auto MaxDurability_it = object.find("MaxDurability");
    if (MaxDurability_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.MaxDurability");
    }
    if (!MaxDurability_it->is_number_integer() && !MaxDurability_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: ItemTable.MaxDurability");
    }
    row.MaxDurability = static_cast<std::int32_t>(MaxDurability_it->get<long long>());
    const auto MaxTradeCount_it = object.find("MaxTradeCount");
    if (MaxTradeCount_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.MaxTradeCount");
    }
    if (!MaxTradeCount_it->is_number_integer() && !MaxTradeCount_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: ItemTable.MaxTradeCount");
    }
    row.MaxTradeCount = static_cast<std::int32_t>(MaxTradeCount_it->get<long long>());
    const auto SellValue_it = object.find("SellValue");
    if (SellValue_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.SellValue");
    }
    if (!SellValue_it->is_number_integer() && !SellValue_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: ItemTable.SellValue");
    }
    row.SellValue = static_cast<std::int32_t>(SellValue_it->get<long long>());
    const auto bCanUseSharedStorage_it = object.find("bCanUseSharedStorage");
    if (bCanUseSharedStorage_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.bCanUseSharedStorage");
    }
    if (!bCanUseSharedStorage_it->is_boolean())
    {
        throw std::runtime_error("bool field must be a boolean: ItemTable.bCanUseSharedStorage");
    }
    row.bCanUseSharedStorage = bCanUseSharedStorage_it->get<bool>();
    const auto bCanBindToConsumableQuickSlot_it = object.find("bCanBindToConsumableQuickSlot");
    if (bCanBindToConsumableQuickSlot_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.bCanBindToConsumableQuickSlot");
    }
    if (!bCanBindToConsumableQuickSlot_it->is_boolean())
    {
        throw std::runtime_error("bool field must be a boolean: ItemTable.bCanBindToConsumableQuickSlot");
    }
    row.bCanBindToConsumableQuickSlot = bCanBindToConsumableQuickSlot_it->get<bool>();
    const auto IconKey_it = object.find("IconKey");
    if (IconKey_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.IconKey");
    }
    if (!IconKey_it->is_string())
    {
        throw std::runtime_error("string field must be a string: ItemTable.IconKey");
    }
    row.IconKey = IconKey_it->get<std::string>();
    const auto BalanceVersion_it = object.find("BalanceVersion");
    if (BalanceVersion_it == object.end())
    {
        throw std::runtime_error("missing required field: ItemTable.BalanceVersion");
    }
    if (!BalanceVersion_it->is_number_integer() && !BalanceVersion_it->is_number_unsigned())
    {
        throw std::runtime_error("integer field must be numeric: ItemTable.BalanceVersion");
    }
    row.BalanceVersion = static_cast<std::int32_t>(BalanceVersion_it->get<long long>());
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
        throw std::runtime_error("failed to open data file");
    }

    nlohmann::json root;
    input >> root;
    return LoadItemTable(root);
}

} // namespace psh::generated::combat
