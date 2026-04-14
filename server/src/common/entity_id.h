#pragma once

#include "types/type_def.h"

#include <concepts>
#include <cstdint>
#include <functional>

namespace psh
{

enum class EnumObjectType : Int8
{
    eNone,
    ePlayer,
    eNpc,
    eItem
};

//  ContentID, DatabaseID로 분리될 수 있음
class EntityId
{
public:
    constexpr EntityId() = default;
    explicit constexpr EntityId(std::uint64_t entityId)
        : entityId_(entityId)
    {
    }
    virtual ~EntityId() = default;

    constexpr explicit operator UInt64() const
    {
        return entityId_;
    }

    [[nodiscard]] auto GetEntityId() const -> EntityId
    {
        return EntityId(entityId_);
    }
    void SetEntityId(const EntityId& entityId)
    {
        entityId_ = static_cast<UInt64>(entityId);
    }
    //
    constexpr EntityId(const EntityId&) = default;
    constexpr EntityId(EntityId&&) = default;

    EntityId& operator=(const EntityId&) = default;
    EntityId& operator=(EntityId&&) = default;

    constexpr bool operator==(const EntityId& other) const = default;

private:
    std::uint64_t entityId_{0};
};

template <std::integral I>
constexpr bool operator==(const EntityId& lhs, I rhs)
{
    return static_cast<std::uint64_t>(lhs) == rhs;
}
template <std::integral I>
constexpr bool operator==(I lhs, const EntityId& rhs)
{
    return lhs == static_cast<std::uint64_t>(rhs);
}

} // namespace psh

template <>
struct std::hash<psh::EntityId>
{
    std::size_t operator()(const psh::EntityId& entityId) const noexcept
    {
        return static_cast<std::size_t>(entityId);
    }
};