#pragma once

#include <cstdint>
#include <concepts>

namespace portpolio::types {

class EntityId
{
public:
    constexpr explicit EntityId() = default;
    constexpr explicit EntityId(std::uint64_t entityId) {
        entityId_ = entityId;
    }

    virtual ~EntityId() = default;

    constexpr explicit EntityId(const EntityId&) = default;
    constexpr explicit EntityId(EntityId&&) = default;

    EntityId& operator=(const EntityId&) = default;
    EntityId& operator=(EntityId&&) = default;
    constexpr explicit operator std::uint64_t() const { return entityId_; }

    friend constexpr bool operator==(const EntityId& lhs, const EntityId& rhs) {return lhs.entityId_ == rhs.entityId_;}
    friend constexpr bool operator!=(const EntityId& lhs, const EntityId& rhs) { return !(lhs == rhs);}

    template <std::integral I> friend constexpr bool operator==(const EntityId& lhs, I rhs) { return static_cast<std::uint64_t>(lhs) == rhs; }
    template <std::integral I> friend constexpr bool operator!=(const EntityId& lhs, I rhs) { return !(lhs == rhs);}
    template <std::integral I> friend constexpr bool operator==(I lhs, const EntityId& rhs) { return lhs == static_cast<std::uint64_t>(rhs); }
    template <std::integral I> friend constexpr bool operator!=(I lhs, const EntityId& rhs) { return !(lhs == rhs);}


private:
    std::uint64_t entityId_{0};
};

}  // namespace portpolio::types

namespace std {
template <>
struct hash<portpolio::types::EntityId> {
    std::size_t operator()(const portpolio::types::EntityId& entityId) const {
        return static_cast<std::size_t>(entityId);
    }
};
} // namespace std