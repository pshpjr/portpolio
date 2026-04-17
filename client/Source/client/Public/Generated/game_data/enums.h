// Auto-generated. Do not edit.
#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EEquipSlot : uint8
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

inline bool TryParseEEquipSlot(const FString& text, EEquipSlot& out_value)
{
    if (text.Equals(TEXT("None")))
    {
        out_value = EEquipSlot::None;
        return true;
    }
    else if (text.Equals(TEXT("Weapon")))
    {
        out_value = EEquipSlot::Weapon;
        return true;
    }
    else if (text.Equals(TEXT("Chest")))
    {
        out_value = EEquipSlot::Chest;
        return true;
    }
    else if (text.Equals(TEXT("Legs")))
    {
        out_value = EEquipSlot::Legs;
        return true;
    }
    else if (text.Equals(TEXT("Hands")))
    {
        out_value = EEquipSlot::Hands;
        return true;
    }
    else if (text.Equals(TEXT("Necklace")))
    {
        out_value = EEquipSlot::Necklace;
        return true;
    }
    else if (text.Equals(TEXT("RingLeft")))
    {
        out_value = EEquipSlot::RingLeft;
        return true;
    }
    else if (text.Equals(TEXT("RingRight")))
    {
        out_value = EEquipSlot::RingRight;
        return true;
    }
    return false;
}

UENUM()
enum class EItemCategory : uint8
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

inline bool TryParseEItemCategory(const FString& text, EItemCategory& out_value)
{
    if (text.Equals(TEXT("None")))
    {
        out_value = EItemCategory::None;
        return true;
    }
    else if (text.Equals(TEXT("Weapon")))
    {
        out_value = EItemCategory::Weapon;
        return true;
    }
    else if (text.Equals(TEXT("Armor")))
    {
        out_value = EItemCategory::Armor;
        return true;
    }
    else if (text.Equals(TEXT("Consumable")))
    {
        out_value = EItemCategory::Consumable;
        return true;
    }
    else if (text.Equals(TEXT("SkillBook")))
    {
        out_value = EItemCategory::SkillBook;
        return true;
    }
    else if (text.Equals(TEXT("Material")))
    {
        out_value = EItemCategory::Material;
        return true;
    }
    else if (text.Equals(TEXT("Quest")))
    {
        out_value = EItemCategory::Quest;
        return true;
    }
    else if (text.Equals(TEXT("Currency")))
    {
        out_value = EItemCategory::Currency;
        return true;
    }
    return false;
}

UENUM()
enum class EItemRarity : uint8
{
    Normal,
    Uncommon,
    Rare,
    Epic
};

inline bool TryParseEItemRarity(const FString& text, EItemRarity& out_value)
{
    if (text.Equals(TEXT("Normal")))
    {
        out_value = EItemRarity::Normal;
        return true;
    }
    else if (text.Equals(TEXT("Uncommon")))
    {
        out_value = EItemRarity::Uncommon;
        return true;
    }
    else if (text.Equals(TEXT("Rare")))
    {
        out_value = EItemRarity::Rare;
        return true;
    }
    else if (text.Equals(TEXT("Epic")))
    {
        out_value = EItemRarity::Epic;
        return true;
    }
    return false;
}

UENUM()
enum class ERangeProfile : uint8
{
    None,
    Melee,
    Ranged
};

inline bool TryParseERangeProfile(const FString& text, ERangeProfile& out_value)
{
    if (text.Equals(TEXT("None")))
    {
        out_value = ERangeProfile::None;
        return true;
    }
    else if (text.Equals(TEXT("Melee")))
    {
        out_value = ERangeProfile::Melee;
        return true;
    }
    else if (text.Equals(TEXT("Ranged")))
    {
        out_value = ERangeProfile::Ranged;
        return true;
    }
    return false;
}

UENUM()
enum class EResourceType : uint8
{
    None,
    Stamina,
    Mana
};

inline bool TryParseEResourceType(const FString& text, EResourceType& out_value)
{
    if (text.Equals(TEXT("None")))
    {
        out_value = EResourceType::None;
        return true;
    }
    else if (text.Equals(TEXT("Stamina")))
    {
        out_value = EResourceType::Stamina;
        return true;
    }
    else if (text.Equals(TEXT("Mana")))
    {
        out_value = EResourceType::Mana;
        return true;
    }
    return false;
}

UENUM()
enum class EWeaponHandType : uint8
{
    None,
    OneHandShield,
    TwoHand,
    Channeling
};

inline bool TryParseEWeaponHandType(const FString& text, EWeaponHandType& out_value)
{
    if (text.Equals(TEXT("None")))
    {
        out_value = EWeaponHandType::None;
        return true;
    }
    else if (text.Equals(TEXT("OneHandShield")))
    {
        out_value = EWeaponHandType::OneHandShield;
        return true;
    }
    else if (text.Equals(TEXT("TwoHand")))
    {
        out_value = EWeaponHandType::TwoHand;
        return true;
    }
    else if (text.Equals(TEXT("Channeling")))
    {
        out_value = EWeaponHandType::Channeling;
        return true;
    }
    return false;
}

UENUM()
enum class EWeaponType : uint8
{
    None,
    SwordShield,
    Greatsword,
    Staff
};

inline bool TryParseEWeaponType(const FString& text, EWeaponType& out_value)
{
    if (text.Equals(TEXT("None")))
    {
        out_value = EWeaponType::None;
        return true;
    }
    else if (text.Equals(TEXT("SwordShield")))
    {
        out_value = EWeaponType::SwordShield;
        return true;
    }
    else if (text.Equals(TEXT("Greatsword")))
    {
        out_value = EWeaponType::Greatsword;
        return true;
    }
    else if (text.Equals(TEXT("Staff")))
    {
        out_value = EWeaponType::Staff;
        return true;
    }
    return false;
}
