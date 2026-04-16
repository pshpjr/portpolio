// Auto-generated. Do not edit.
#pragma once

#include <cstdint>

namespace psh::generated {

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

enum class EItemRarity : std::uint8_t
{
    Normal,
    Uncommon,
    Rare,
    Epic
};

enum class ERangeProfile : std::uint8_t
{
    None,
    Melee,
    Ranged
};

enum class EResourceType : std::uint8_t
{
    None,
    Stamina,
    Mana
};

enum class EWeaponHandType : std::uint8_t
{
    None,
    OneHandShield,
    TwoHand,
    Channeling
};

enum class EWeaponType : std::uint8_t
{
    None,
    SwordShield,
    Greatsword,
    Staff
};

} // namespace psh::generated