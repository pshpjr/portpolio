// Auto-generated. Do not edit.
#pragma once

#include <cstdint>
#include <string_view>

namespace psh::generated::combat {

enum class EEquipSlot : std::uint8_t
{
    None,
    Weapon,
    Chest,
    Legs,
    Hands,
    Necklace,
    RingLeft,
    RingRight
};

inline bool TryParseEEquipSlot(std::string_view text, EEquipSlot& out_value)
{
    if (text == "None")
    {
        out_value = EEquipSlot::None;
        return true;
    }
    else if (text == "Weapon")
    {
        out_value = EEquipSlot::Weapon;
        return true;
    }
    else if (text == "Chest")
    {
        out_value = EEquipSlot::Chest;
        return true;
    }
    else if (text == "Legs")
    {
        out_value = EEquipSlot::Legs;
        return true;
    }
    else if (text == "Hands")
    {
        out_value = EEquipSlot::Hands;
        return true;
    }
    else if (text == "Necklace")
    {
        out_value = EEquipSlot::Necklace;
        return true;
    }
    else if (text == "RingLeft")
    {
        out_value = EEquipSlot::RingLeft;
        return true;
    }
    else if (text == "RingRight")
    {
        out_value = EEquipSlot::RingRight;
        return true;
    }
    return false;
}

enum class EItemCategory : std::uint8_t
{
    None,
    Weapon,
    Armor,
    Consumable,
    SkillBook,
    Material,
    Quest,
    Currency
};

inline bool TryParseEItemCategory(std::string_view text, EItemCategory& out_value)
{
    if (text == "None")
    {
        out_value = EItemCategory::None;
        return true;
    }
    else if (text == "Weapon")
    {
        out_value = EItemCategory::Weapon;
        return true;
    }
    else if (text == "Armor")
    {
        out_value = EItemCategory::Armor;
        return true;
    }
    else if (text == "Consumable")
    {
        out_value = EItemCategory::Consumable;
        return true;
    }
    else if (text == "SkillBook")
    {
        out_value = EItemCategory::SkillBook;
        return true;
    }
    else if (text == "Material")
    {
        out_value = EItemCategory::Material;
        return true;
    }
    else if (text == "Quest")
    {
        out_value = EItemCategory::Quest;
        return true;
    }
    else if (text == "Currency")
    {
        out_value = EItemCategory::Currency;
        return true;
    }
    return false;
}

enum class EItemRarity : std::uint8_t
{
    Normal,
    Uncommon,
    Rare,
    Epic
};

inline bool TryParseEItemRarity(std::string_view text, EItemRarity& out_value)
{
    if (text == "Normal")
    {
        out_value = EItemRarity::Normal;
        return true;
    }
    else if (text == "Uncommon")
    {
        out_value = EItemRarity::Uncommon;
        return true;
    }
    else if (text == "Rare")
    {
        out_value = EItemRarity::Rare;
        return true;
    }
    else if (text == "Epic")
    {
        out_value = EItemRarity::Epic;
        return true;
    }
    return false;
}

enum class ERangeProfile : std::uint8_t
{
    None,
    Melee,
    Ranged
};

inline bool TryParseERangeProfile(std::string_view text, ERangeProfile& out_value)
{
    if (text == "None")
    {
        out_value = ERangeProfile::None;
        return true;
    }
    else if (text == "Melee")
    {
        out_value = ERangeProfile::Melee;
        return true;
    }
    else if (text == "Ranged")
    {
        out_value = ERangeProfile::Ranged;
        return true;
    }
    return false;
}

enum class EResourceType : std::uint8_t
{
    None,
    Stamina,
    Mana
};

inline bool TryParseEResourceType(std::string_view text, EResourceType& out_value)
{
    if (text == "None")
    {
        out_value = EResourceType::None;
        return true;
    }
    else if (text == "Stamina")
    {
        out_value = EResourceType::Stamina;
        return true;
    }
    else if (text == "Mana")
    {
        out_value = EResourceType::Mana;
        return true;
    }
    return false;
}

enum class EWeaponHandType : std::uint8_t
{
    None,
    OneHandShield,
    TwoHand,
    Channeling
};

inline bool TryParseEWeaponHandType(std::string_view text, EWeaponHandType& out_value)
{
    if (text == "None")
    {
        out_value = EWeaponHandType::None;
        return true;
    }
    else if (text == "OneHandShield")
    {
        out_value = EWeaponHandType::OneHandShield;
        return true;
    }
    else if (text == "TwoHand")
    {
        out_value = EWeaponHandType::TwoHand;
        return true;
    }
    else if (text == "Channeling")
    {
        out_value = EWeaponHandType::Channeling;
        return true;
    }
    return false;
}

enum class EWeaponType : std::uint8_t
{
    None,
    SwordShield,
    Greatsword,
    Staff
};

inline bool TryParseEWeaponType(std::string_view text, EWeaponType& out_value)
{
    if (text == "None")
    {
        out_value = EWeaponType::None;
        return true;
    }
    else if (text == "SwordShield")
    {
        out_value = EWeaponType::SwordShield;
        return true;
    }
    else if (text == "Greatsword")
    {
        out_value = EWeaponType::Greatsword;
        return true;
    }
    else if (text == "Staff")
    {
        out_value = EWeaponType::Staff;
        return true;
    }
    return false;
}

} // namespace psh::generated::combat
