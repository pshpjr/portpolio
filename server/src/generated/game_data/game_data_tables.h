// Auto-generated. Do not edit.
#pragma once

#include <array>
#include <filesystem>
#include <string_view>
#include <vector>

#include "table_common.h"
#include "enums.h"
#include "item_table.h"
#include "player_stat_table.h"
#include "user_level_stat_link_table.h"
#include "weapon_level_stat_link_table.h"
#include "weapon_stat_table.h"
#include "weapon_table.h"

namespace psh::generated {

inline constexpr std::array<TableDescriptor, 6> kGameDataTables = {
    TableDescriptor{"ItemTable", "item_table.schema.json", "item_table.data.json", 5, 15},
    TableDescriptor{"PlayerStatTable", "player_stat_table.schema.json", "player_stat_table.data.json", 40, 11},
    TableDescriptor{"UserLevelStatLinkTable", "user_level_stat_link_table.schema.json", "user_level_stat_link_table.data.json", 40, 3},
    TableDescriptor{"WeaponLevelStatLinkTable", "weapon_level_stat_link_table.schema.json", "weapon_level_stat_link_table.data.json", 75, 4},
    TableDescriptor{"WeaponStatTable", "weapon_stat_table.schema.json", "weapon_stat_table.data.json", 75, 15},
    TableDescriptor{"WeaponTable", "weapon_table.schema.json", "weapon_table.data.json", 3, 22},
};

struct GameDataBundle
{
    std::vector<ItemTableRow> ItemTableRows;
    std::vector<PlayerStatTableRow> PlayerStatTableRows;
    std::vector<UserLevelStatLinkTableRow> UserLevelStatLinkTableRows;
    std::vector<WeaponLevelStatLinkTableRow> WeaponLevelStatLinkTableRows;
    std::vector<WeaponStatTableRow> WeaponStatTableRows;
    std::vector<WeaponTableRow> WeaponTableRows;
};

inline GameDataBundle LoadGameDataBundleFromDirectory(const std::filesystem::path& directory)
{
    GameDataBundle bundle{};
    bundle.ItemTableRows = LoadItemTableFromFile(directory / "item_table.data.json");
    bundle.PlayerStatTableRows = LoadPlayerStatTableFromFile(directory / "player_stat_table.data.json");
    bundle.UserLevelStatLinkTableRows = LoadUserLevelStatLinkTableFromFile(directory / "user_level_stat_link_table.data.json");
    bundle.WeaponLevelStatLinkTableRows = LoadWeaponLevelStatLinkTableFromFile(directory / "weapon_level_stat_link_table.data.json");
    bundle.WeaponStatTableRows = LoadWeaponStatTableFromFile(directory / "weapon_stat_table.data.json");
    bundle.WeaponTableRows = LoadWeaponTableFromFile(directory / "weapon_table.data.json");
    return bundle;
}

} // namespace psh::generated