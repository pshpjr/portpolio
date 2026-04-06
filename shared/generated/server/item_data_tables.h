// Auto-generated. Do not edit.
#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "item/item_table.h"

namespace psh::generated::item {

struct FTableDescriptor
{
    std::string_view TableName;
    std::string_view SchemaFile;
    std::string_view DataFile;
    std::size_t RowCount;
    std::size_t FieldCount;
};

inline constexpr std::array<FTableDescriptor, 1> kItemDataTables = {
    FTableDescriptor{"ItemTable", "item_table.schema.json", "item_table.data.json", 5, 15},
};

struct ItemDataBundle
{
    std::vector<ItemTableRow> ItemTableRows;
};

inline bool LoadItemDataBundleFromDirectory(const std::filesystem::path& Directory, ItemDataBundle& OutBundle, std::string& OutError)
{
    return LoadItemTableFromFile((Directory / "item_table.data.json").string(), OutBundle.ItemTableRows, OutError);
}

} // namespace psh::generated::item
