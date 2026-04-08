#pragma once

#include "types/type_def.h"

#include <strong_type/strong_type.hpp>

#include <concepts>
#include <utility>

namespace psh::lib::types {

template <typename Tag, typename ValueType = Int32>
    requires std::integral<ValueType>
using StrongId =
    strong::type<ValueType,
                 Tag,
                 strong::regular,
                 strong::boolean>;

template <typename Tag, typename ValueType = Int32>
    requires std::integral<ValueType>
[[nodiscard]] constexpr ValueType& ValueOf(StrongId<Tag, ValueType>& value)
    noexcept {
    return value_of(value);
}

template <typename Tag, typename ValueType = Int32>
    requires std::integral<ValueType>
[[nodiscard]] constexpr const ValueType& ValueOf(
    const StrongId<Tag, ValueType>& value) noexcept {
    return value_of(value);
}

template <typename Tag, typename ValueType = Int32>
    requires std::integral<ValueType>
[[nodiscard]] constexpr ValueType&& ValueOf(StrongId<Tag, ValueType>&& value)
    noexcept {
    return value_of(std::move(value));
}

template <typename Tag, typename ValueType = Int32>
    requires std::integral<ValueType>
[[nodiscard]] constexpr const ValueType&& ValueOf(
    const StrongId<Tag, ValueType>&& value) noexcept {
    return value_of(std::move(value));
}

}  // namespace psh::lib::types

template <typename Tag, typename ValueType>
struct std::hash<psh::lib::types::StrongId<Tag, ValueType>> {
    size_t operator()(
        const psh::lib::types::StrongId<Tag, ValueType>& value) const noexcept {
        return std::hash<ValueType>{}(psh::lib::types::ValueOf(value));
    }
};
