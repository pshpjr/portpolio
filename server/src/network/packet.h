#pragma once

#include "network/packet_header.h"

#include <boost/asio/buffer.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

namespace psh::network
{

class Packet
{
public:
    static constexpr std::size_t kSboCapacity = 256;

    // 송신용: protocol enum + payload → 헤더 자동 부착
    template <typename ProtocolEnum>
        requires std::is_enum_v<ProtocolEnum>
              && (sizeof(std::underlying_type_t<ProtocolEnum>) == sizeof(UInt16))
    static std::shared_ptr<Packet> Create(ProtocolEnum protocol, const void* payload, std::size_t payloadSize)
    {
        if (!payload)
            return {};
        return std::shared_ptr<Packet>(
            new Packet(static_cast<UInt16>(protocol), payload, payloadSize));
    }

    // 수신용: 헤더 포함 raw 복사 (SBO 적용)
    static std::shared_ptr<Packet> FromRaw(const char* data, std::size_t length)
    {
        return std::shared_ptr<Packet>(new Packet(data, length));
    }

    [[nodiscard]] const PacketHeader& Header() const noexcept
    {
        return *reinterpret_cast<const PacketHeader*>(DataPtr());
    }

    [[nodiscard]] UInt16 Protocol() const noexcept { return Header().protocol; }

    [[nodiscard]] const char* Data() const noexcept { return DataPtr(); }
    [[nodiscard]] std::size_t Size() const noexcept { return size_; }

    [[nodiscard]] const char* Payload() const noexcept { return DataPtr() + kHeaderSize; }
    [[nodiscard]] std::size_t PayloadSize() const noexcept { return size_ - kHeaderSize; }

    [[nodiscard]] bool UseSbo() const noexcept { return useSbo_; }

    [[nodiscard]] boost::asio::const_buffer AsBuffer() const noexcept
    {
        return boost::asio::buffer(DataPtr(), size_);
    }

    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;
    Packet(Packet&&) = default;
    Packet& operator=(Packet&&) = default;

private:
    [[nodiscard]] const char* DataPtr() const noexcept
    {
        return useSbo_ ? smallBuffer_.data() : heapBuffer_.data();
    }

    [[nodiscard]] char* DataPtr() noexcept
    {
        return useSbo_ ? smallBuffer_.data() : heapBuffer_.data();
    }

    // 송신용
    explicit Packet(UInt16 protocol, const void* payload, std::size_t payloadSize)
        : size_(kHeaderSize + payloadSize)
        , useSbo_(size_ <= kSboCapacity)
    {
        PacketHeader header{};
        header.magic = kPacketMagic;
        header.length = static_cast<UInt16>(size_);
        header.protocol = protocol;

        if (useSbo_)
        {
            std::memcpy(smallBuffer_.data(), &header, kHeaderSize);
            if (payloadSize > 0)
            {
                std::memcpy(smallBuffer_.data() + kHeaderSize, payload, payloadSize);
            }
        }
        else
        {
            heapBuffer_.resize(size_);
            std::memcpy(heapBuffer_.data(), &header, kHeaderSize);
            if (payloadSize > 0)
            {
                std::memcpy(heapBuffer_.data() + kHeaderSize, payload, payloadSize);
            }
        }
    }

    // 수신용: 헤더 포함 raw 복사
    explicit Packet(const char* rawData, std::size_t length)
        : size_(length)
        , useSbo_(length <= kSboCapacity)
    {
        if (useSbo_)
        {
            std::memcpy(smallBuffer_.data(), rawData, length);
        }
        else
        {
            heapBuffer_.assign(rawData, rawData + length);
        }
    }

    std::size_t size_{};
    bool useSbo_{true};
    std::array<char, kSboCapacity> smallBuffer_{};
    std::vector<char> heapBuffer_;
};

} // namespace psh::network
