//
// Created by pshpjr on 26. 3. 30..
//

#include "packet.h"

#include "const_def.h"

#include <cstring>

namespace psh::lib::network
{

auto Packet::CheckLength(const char* byte, size_t length) -> std::pair<bool, size_t>
{
    if (length < sizeof(PacketHeader))
    {
        return {false, 0};
    }

    PacketHeader header{};
    std::memcpy(&header, byte, sizeof(header));

    if (!IsValid(header))
    {
        return {false, 0};
    }

    const size_t packetLength = sizeof(PacketHeader) + header.length;
    if (length < packetLength)
    {
        return {false, 0};
    }

    return {true, header.length};
}

bool Packet::IsValid(const PacketHeader& header)
{
    if (header.magicNumber != 0x1310)
    {
        return false;
    }

    return header.length <= (kMaxPacketSize - sizeof(PacketHeader));
}

} // namespace psh::lib::network
