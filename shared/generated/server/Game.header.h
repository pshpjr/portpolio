// Auto-generated. Do not edit.
#pragma once

#include <concepts>
#include <type_traits>

#include "Protobuf/Game.packet.pb.h"
#include "Protobuf/Game.struct.pb.h"

namespace Game {
enum class PACKET_TYPE
{
    NONE,
};

template <typename T> inline constexpr PACKET_TYPE kPacketType = PACKET_TYPE::NONE;
} // namespace Game


// Concepts
template <class T>
concept IsGame = (Game::kPacketType<std::remove_cvref_t<T>> != Game::PACKET_TYPE::NONE);

template <class T>
inline constexpr bool exactly_Game = IsGame<T>;


// Traits
struct Game_Traits
{
    using PacketType = Game::PACKET_TYPE;

    template <typename T>
    static consteval PacketType GetType()
    {
        using Msg = std::remove_cvref_t<T>;
        return Game::kPacketType<Msg>;
    }
};
