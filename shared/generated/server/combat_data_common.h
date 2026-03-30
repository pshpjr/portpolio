// Auto-generated. Do not edit.
#pragma once

#include <cstddef>
#include <string_view>

namespace psh::generated::combat {
struct TableFieldMetadata
{
    std::string_view Name;
    std::string_view Type;
    bool bRequired;
    bool bKey;
};

struct TableDescriptor
{
    std::string_view TableName;
    std::string_view SchemaFile;
    std::string_view DataFile;
    std::size_t RowCount;
    std::size_t FieldCount;
};

} // namespace psh::generated::combat
