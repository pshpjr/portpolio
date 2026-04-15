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
    RqLogin,
    RsLogin,
    RqUserInfo,
    RsUserInfo,
    RqUserOutfit,
};

template <typename T> inline constexpr PACKET_TYPE kPacketType           = PACKET_TYPE::NONE;
template <> inline constexpr PACKET_TYPE kPacketType<Game::RqLogin>      = PACKET_TYPE::RqLogin;
template <> inline constexpr PACKET_TYPE kPacketType<Game::RsLogin>      = PACKET_TYPE::RsLogin;
template <> inline constexpr PACKET_TYPE kPacketType<Game::RqUserInfo>   = PACKET_TYPE::RqUserInfo;
template <> inline constexpr PACKET_TYPE kPacketType<Game::RsUserInfo>   = PACKET_TYPE::RsUserInfo;
template <> inline constexpr PACKET_TYPE kPacketType<Game::RqUserOutfit> = PACKET_TYPE::RqUserOutfit;
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
