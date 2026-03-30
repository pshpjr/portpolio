//
// Created by pshpjr on 26. 3. 30..
//

#ifndef PORTPOLIO_PACKET_H
#define PORTPOLIO_PACKET_H
#include <cstddef>
#include <cstdint>
#include <utility>

namespace psh::lib::network
{
struct PacketHeader
{
    uint16_t magicNumber = 0x1310;
    uint16_t length; // 헤더 제외
};

class Packet
{
  public:
    static auto CheckLength(const char* byte, size_t length) -> std::pair<bool, size_t>;
    static bool IsValid(const PacketHeader& header);
};

} // namespace psh::lib::network

#endif // PORTPOLIO_PACKET_H
