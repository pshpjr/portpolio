// Auto-generated. Do not edit.
#pragma once

#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace psh::generated::item {

// ============================================================
// Enums
// ============================================================
enum class EItemCategory : std::uint8_t
{
    None = 0,
    Weapon,
    Armor,
    Consumable,
    SkillBook,
    Material,
    Quest,
    Currency
};

inline bool TryParseEItemCategory(const std::string& Value, EItemCategory& Out)
{
    if (Value == "None")
    {
        Out = EItemCategory::None;
        return true;
    }
    else if (Value == "Weapon")
    {
        Out = EItemCategory::Weapon;
        return true;
    }
    else if (Value == "Armor")
    {
        Out = EItemCategory::Armor;
        return true;
    }
    else if (Value == "Consumable")
    {
        Out = EItemCategory::Consumable;
        return true;
    }
    else if (Value == "SkillBook")
    {
        Out = EItemCategory::SkillBook;
        return true;
    }
    else if (Value == "Material")
    {
        Out = EItemCategory::Material;
        return true;
    }
    else if (Value == "Quest")
    {
        Out = EItemCategory::Quest;
        return true;
    }
    else if (Value == "Currency")
    {
        Out = EItemCategory::Currency;
        return true;
    }
    return false;
}

enum class EItemRarity : std::uint8_t
{
    Normal = 0,
    Uncommon,
    Rare,
    Epic
};

inline bool TryParseEItemRarity(const std::string& Value, EItemRarity& Out)
{
    if (Value == "Normal")
    {
        Out = EItemRarity::Normal;
        return true;
    }
    else if (Value == "Uncommon")
    {
        Out = EItemRarity::Uncommon;
        return true;
    }
    else if (Value == "Rare")
    {
        Out = EItemRarity::Rare;
        return true;
    }
    else if (Value == "Epic")
    {
        Out = EItemRarity::Epic;
        return true;
    }
    return false;
}

enum class EEquipSlot : std::uint8_t
{
    None = 0,
    Weapon,
    Chest,
    Legs,
    Hands,
    Necklace,
    RingLeft,
    RingRight
};

inline bool TryParseEEquipSlot(const std::string& Value, EEquipSlot& Out)
{
    if (Value == "None")
    {
        Out = EEquipSlot::None;
        return true;
    }
    else if (Value == "Weapon")
    {
        Out = EEquipSlot::Weapon;
        return true;
    }
    else if (Value == "Chest")
    {
        Out = EEquipSlot::Chest;
        return true;
    }
    else if (Value == "Legs")
    {
        Out = EEquipSlot::Legs;
        return true;
    }
    else if (Value == "Hands")
    {
        Out = EEquipSlot::Hands;
        return true;
    }
    else if (Value == "Necklace")
    {
        Out = EEquipSlot::Necklace;
        return true;
    }
    else if (Value == "RingLeft")
    {
        Out = EEquipSlot::RingLeft;
        return true;
    }
    else if (Value == "RingRight")
    {
        Out = EEquipSlot::RingRight;
        return true;
    }
    return false;
}

enum class EWeaponType : std::uint8_t
{
    None = 0,
    SwordShield,
    Greatsword,
    Staff
};

inline bool TryParseEWeaponType(const std::string& Value, EWeaponType& Out)
{
    if (Value == "None")
    {
        Out = EWeaponType::None;
        return true;
    }
    else if (Value == "SwordShield")
    {
        Out = EWeaponType::SwordShield;
        return true;
    }
    else if (Value == "Greatsword")
    {
        Out = EWeaponType::Greatsword;
        return true;
    }
    else if (Value == "Staff")
    {
        Out = EWeaponType::Staff;
        return true;
    }
    return false;
}

// ============================================================
// Row Struct
// ============================================================
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

// ============================================================
// JSON Loaders
// ============================================================
inline bool LoadItemTableRow(const nlohmann::json& Object, ItemTableRow& OutRow, std::string& OutError)
{
    if (!Object.is_object())
    {
        OutError = "table row must be a JSON object";
        return false;
    }

    const auto ItemIdIt = Object.find("ItemId");
    if (ItemIdIt == Object.end())
    {
        OutError = "missing required numeric field: ItemTable.ItemId";
        return false;
    }
    if (!ItemIdIt->is_number_integer() && !ItemIdIt->is_number_unsigned())
    {
        OutError = "integer field must be numeric: ItemTable.ItemId";
        return false;
    }
    OutRow.ItemId = static_cast<std::int32_t>(ItemIdIt->get<long long>());
    const auto ItemCodeIt = Object.find("ItemCode");
    if (ItemCodeIt == Object.end())
    {
        OutError = "missing required string field: ItemTable.ItemCode";
        return false;
    }
    if (!ItemCodeIt->is_string())
    {
        OutError = "string field must be a string: ItemTable.ItemCode";
        return false;
    }
    OutRow.ItemCode = ItemCodeIt->get<std::string>();
    const auto ItemNameIt = Object.find("ItemName");
    if (ItemNameIt == Object.end())
    {
        OutError = "missing required string field: ItemTable.ItemName";
        return false;
    }
    if (!ItemNameIt->is_string())
    {
        OutError = "string field must be a string: ItemTable.ItemName";
        return false;
    }
    OutRow.ItemName = ItemNameIt->get<std::string>();
    const auto ItemCategoryIt = Object.find("ItemCategory");
    if (ItemCategoryIt == Object.end())
    {
        OutError = "missing required enum field: ItemTable.ItemCategory";
        return false;
    }
    if (!ItemCategoryIt->is_string())
    {
        OutError = "enum field must be a string: ItemTable.ItemCategory";
        return false;
    }
    if (!TryParseEItemCategory(ItemCategoryIt->get<std::string>(), OutRow.ItemCategory))
    {
        OutError = "failed to parse enum field: ItemTable.ItemCategory";
        return false;
    }
    const auto RarityIt = Object.find("Rarity");
    if (RarityIt == Object.end())
    {
        OutError = "missing required enum field: ItemTable.Rarity";
        return false;
    }
    if (!RarityIt->is_string())
    {
        OutError = "enum field must be a string: ItemTable.Rarity";
        return false;
    }
    if (!TryParseEItemRarity(RarityIt->get<std::string>(), OutRow.Rarity))
    {
        OutError = "failed to parse enum field: ItemTable.Rarity";
        return false;
    }
    const auto EquipSlotIt = Object.find("EquipSlot");
    if (EquipSlotIt == Object.end())
    {
        OutError = "missing required enum field: ItemTable.EquipSlot";
        return false;
    }
    if (!EquipSlotIt->is_string())
    {
        OutError = "enum field must be a string: ItemTable.EquipSlot";
        return false;
    }
    if (!TryParseEEquipSlot(EquipSlotIt->get<std::string>(), OutRow.EquipSlot))
    {
        OutError = "failed to parse enum field: ItemTable.EquipSlot";
        return false;
    }
    const auto WeaponTypeIt = Object.find("WeaponType");
    if (WeaponTypeIt == Object.end())
    {
        OutError = "missing required enum field: ItemTable.WeaponType";
        return false;
    }
    if (!WeaponTypeIt->is_string())
    {
        OutError = "enum field must be a string: ItemTable.WeaponType";
        return false;
    }
    if (!TryParseEWeaponType(WeaponTypeIt->get<std::string>(), OutRow.WeaponType))
    {
        OutError = "failed to parse enum field: ItemTable.WeaponType";
        return false;
    }
    const auto MaxStackCountIt = Object.find("MaxStackCount");
    if (MaxStackCountIt == Object.end())
    {
        OutError = "missing required numeric field: ItemTable.MaxStackCount";
        return false;
    }
    if (!MaxStackCountIt->is_number_integer() && !MaxStackCountIt->is_number_unsigned())
    {
        OutError = "integer field must be numeric: ItemTable.MaxStackCount";
        return false;
    }
    OutRow.MaxStackCount = static_cast<std::int32_t>(MaxStackCountIt->get<long long>());
    const auto MaxDurabilityIt = Object.find("MaxDurability");
    if (MaxDurabilityIt == Object.end())
    {
        OutError = "missing required numeric field: ItemTable.MaxDurability";
        return false;
    }
    if (!MaxDurabilityIt->is_number_integer() && !MaxDurabilityIt->is_number_unsigned())
    {
        OutError = "integer field must be numeric: ItemTable.MaxDurability";
        return false;
    }
    OutRow.MaxDurability = static_cast<std::int32_t>(MaxDurabilityIt->get<long long>());
    const auto MaxTradeCountIt = Object.find("MaxTradeCount");
    if (MaxTradeCountIt == Object.end())
    {
        OutError = "missing required numeric field: ItemTable.MaxTradeCount";
        return false;
    }
    if (!MaxTradeCountIt->is_number_integer() && !MaxTradeCountIt->is_number_unsigned())
    {
        OutError = "integer field must be numeric: ItemTable.MaxTradeCount";
        return false;
    }
    OutRow.MaxTradeCount = static_cast<std::int32_t>(MaxTradeCountIt->get<long long>());
    const auto SellValueIt = Object.find("SellValue");
    if (SellValueIt == Object.end())
    {
        OutError = "missing required numeric field: ItemTable.SellValue";
        return false;
    }
    if (!SellValueIt->is_number_integer() && !SellValueIt->is_number_unsigned())
    {
        OutError = "integer field must be numeric: ItemTable.SellValue";
        return false;
    }
    OutRow.SellValue = static_cast<std::int32_t>(SellValueIt->get<long long>());
    const auto bCanUseSharedStorageIt = Object.find("bCanUseSharedStorage");
    if (bCanUseSharedStorageIt == Object.end())
    {
        OutError = "missing required bool field: ItemTable.bCanUseSharedStorage";
        return false;
    }
    if (!bCanUseSharedStorageIt->is_boolean())
    {
        OutError = "bool field must be a boolean: ItemTable.bCanUseSharedStorage";
        return false;
    }
    OutRow.bCanUseSharedStorage = bCanUseSharedStorageIt->get<bool>();
    const auto bCanBindToConsumableQuickSlotIt = Object.find("bCanBindToConsumableQuickSlot");
    if (bCanBindToConsumableQuickSlotIt == Object.end())
    {
        OutError = "missing required bool field: ItemTable.bCanBindToConsumableQuickSlot";
        return false;
    }
    if (!bCanBindToConsumableQuickSlotIt->is_boolean())
    {
        OutError = "bool field must be a boolean: ItemTable.bCanBindToConsumableQuickSlot";
        return false;
    }
    OutRow.bCanBindToConsumableQuickSlot = bCanBindToConsumableQuickSlotIt->get<bool>();
    const auto IconKeyIt = Object.find("IconKey");
    if (IconKeyIt == Object.end())
    {
        OutError = "missing required string field: ItemTable.IconKey";
        return false;
    }
    if (!IconKeyIt->is_string())
    {
        OutError = "string field must be a string: ItemTable.IconKey";
        return false;
    }
    OutRow.IconKey = IconKeyIt->get<std::string>();
    const auto BalanceVersionIt = Object.find("BalanceVersion");
    if (BalanceVersionIt == Object.end())
    {
        OutError = "missing required numeric field: ItemTable.BalanceVersion";
        return false;
    }
    if (!BalanceVersionIt->is_number_integer() && !BalanceVersionIt->is_number_unsigned())
    {
        OutError = "integer field must be numeric: ItemTable.BalanceVersion";
        return false;
    }
    OutRow.BalanceVersion = static_cast<std::int32_t>(BalanceVersionIt->get<long long>());
    return true;
}

inline bool LoadItemTable(const std::string& JsonText, std::vector<ItemTableRow>& OutRows, std::string& OutError)
{
    const nlohmann::json Root = nlohmann::json::parse(JsonText, nullptr, false);
    if (Root.is_discarded())
    {
        OutError = "failed to parse table JSON";
        return false;
    }

    const nlohmann::json* Rows = nullptr;
    if (Root.is_array())
    {
        Rows = &Root;
    }
    else if (Root.is_object() && Root.contains("rows"))
    {
        Rows = &Root.at("rows");
    }
    else if (Root.is_object() && Root.contains("data"))
    {
        Rows = &Root.at("data");
    }

    if (Rows == nullptr || !Rows->is_array())
    {
        OutError = "table root must be an array or an object with rows/data";
        return false;
    }

    OutRows.clear();
    OutRows.reserve(Rows->size());
    for (const auto& Entry : *Rows)
    {
        ItemTableRow Row{};
        if (!LoadItemTableRow(Entry, Row, OutError))
        {
            return false;
        }
        OutRows.push_back(std::move(Row));
    }
    return true;
}

inline bool LoadItemTableFromFile(const std::string& FilePath, std::vector<ItemTableRow>& OutRows, std::string& OutError)
{
    std::ifstream Input(FilePath);
    if (!Input.is_open())
    {
        OutError = "failed to open data file";
        return false;
    }

    std::ostringstream Buffer;
    Buffer << Input.rdbuf();
    return LoadItemTable(Buffer.str(), OutRows, OutError);
}

// ============================================================
// Index Builders
// ============================================================
inline std::unordered_map<std::int32_t, ItemTableRow> BuildItemTableIndexByItemId(
    const std::vector<ItemTableRow>& Rows)
{
    std::unordered_map<std::int32_t, ItemTableRow> Index;
    for (const auto& Row : Rows)
    {
        Index[Row.ItemId] = Row;
    }
    return Index;
}

} // namespace psh::generated::item
