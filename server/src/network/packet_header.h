#pragma once

#include "types/type_def.h"

#include <cstddef>

namespace psh::network
{

static constexpr UInt16 kPacketMagic = 0x504B; // 'PK'

#pragma pack(push, 1)
struct PacketHeader
{
    UInt16 magic;
    UInt16 length;   // 헤더 포함 전체 패킷 크기
    UInt16 protocol; // 패킷 종류 식별자
};
#pragma pack(pop)

static constexpr std::size_t kHeaderSize = sizeof(PacketHeader);
static constexpr UInt16 kMaxPacketSize = 8192;

} // namespace psh::network
