// Auto-generated. Do not edit.
#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"

#include "table_common.h"
#include "enums.h"
#include "item_table.h"
#include "player_stat_table.h"
#include "user_level_stat_link_table.h"
#include "weapon_level_stat_link_table.h"
#include "weapon_stat_table.h"
#include "weapon_table.h"

inline constexpr std::array<FTableDescriptor, 6> kGameDataTables = {
    FTableDescriptor{TEXT("ItemTable"), TEXT("item_table.schema.json"), TEXT("item_table.data.json"), 5, 15},
    FTableDescriptor{TEXT("PlayerStatTable"), TEXT("player_stat_table.schema.json"), TEXT("player_stat_table.data.json"), 40, 11},
    FTableDescriptor{TEXT("UserLevelStatLinkTable"), TEXT("user_level_stat_link_table.schema.json"), TEXT("user_level_stat_link_table.data.json"), 40, 3},
    FTableDescriptor{TEXT("WeaponLevelStatLinkTable"), TEXT("weapon_level_stat_link_table.schema.json"), TEXT("weapon_level_stat_link_table.data.json"), 75, 4},
    FTableDescriptor{TEXT("WeaponStatTable"), TEXT("weapon_stat_table.schema.json"), TEXT("weapon_stat_table.data.json"), 75, 15},
    FTableDescriptor{TEXT("WeaponTable"), TEXT("weapon_table.schema.json"), TEXT("weapon_table.data.json"), 3, 22},
};

struct FGameDataBundle
{
    TArray<FItemTableRow> ItemTableRows;
    TArray<FPlayerStatTableRow> PlayerStatTableRows;
    TArray<FUserLevelStatLinkTableRow> UserLevelStatLinkTableRows;
    TArray<FWeaponLevelStatLinkTableRow> WeaponLevelStatLinkTableRows;
    TArray<FWeaponStatTableRow> WeaponStatTableRows;
    TArray<FWeaponTableRow> WeaponTableRows;
};

inline bool LoadGameDataBundleFromDirectory(const FString& directory, FGameDataBundle& out_bundle, FString& out_error)
{
    if (!LoadItemTableFromFile(FPaths::Combine(directory, TEXT("item_table.data.json")), out_bundle.ItemTableRows, out_error))
    {
        return false;
    }
    if (!LoadPlayerStatTableFromFile(FPaths::Combine(directory, TEXT("player_stat_table.data.json")), out_bundle.PlayerStatTableRows, out_error))
    {
        return false;
    }
    if (!LoadUserLevelStatLinkTableFromFile(FPaths::Combine(directory, TEXT("user_level_stat_link_table.data.json")), out_bundle.UserLevelStatLinkTableRows, out_error))
    {
        return false;
    }
    if (!LoadWeaponLevelStatLinkTableFromFile(FPaths::Combine(directory, TEXT("weapon_level_stat_link_table.data.json")), out_bundle.WeaponLevelStatLinkTableRows, out_error))
    {
        return false;
    }
    if (!LoadWeaponStatTableFromFile(FPaths::Combine(directory, TEXT("weapon_stat_table.data.json")), out_bundle.WeaponStatTableRows, out_error))
    {
        return false;
    }
    if (!LoadWeaponTableFromFile(FPaths::Combine(directory, TEXT("weapon_table.data.json")), out_bundle.WeaponTableRows, out_error))
    {
        return false;
    }
    return true;
}