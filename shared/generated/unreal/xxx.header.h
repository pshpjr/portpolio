// Auto-generated. Do not edit.
#pragma once

#include <concepts>
#include <type_traits>

#include "Protobuf/xxx.Common.pb.h"

namespace xxx {
enum class PACKET_TYPE
{
    NONE,
};

template <typename T> inline constexpr PACKET_TYPE kPacketType = PACKET_TYPE::NONE;
} // namespace xxx


// Concepts
template <class T>
concept Isxxx = (xxx::kPacketType<std::remove_cvref_t<T>> != xxx::PACKET_TYPE::NONE);

template <class T>
inline constexpr bool exactly_xxx = Isxxx<T>;


// Traits
struct xxx_Traits
{
    using PacketType = xxx::PACKET_TYPE;

    template <typename T>
    static consteval PacketType GetType()
    {
        using Msg = std::remove_cvref_t<T>;
        return xxx::kPacketType<Msg>;
    }
};
