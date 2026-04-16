#pragma once

#include <boost/asio/buffer.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace psh::network
{

// 수신 전용 선형 버퍼.
// async_read_some 결과를 commit 하고, 처리 완료분을 consume 한다.
// memmove 기반 — 단순하고 캐시 친화적.
class RecvBuffer
{
public:
    static constexpr std::size_t kDefaultCapacity = 16 * 1024;

    explicit RecvBuffer(std::size_t capacity = kDefaultCapacity)
        : buffer_(capacity)
    {
    }

    // 아직 처리되지 않은 수신 데이터의 시작 포인터. 비어있으면 nullptr.
    [[nodiscard]] const char* Data() const
    {
        return length_ == 0 ? nullptr : buffer_.data();
    }

    [[nodiscard]] char* Data()
    {
        return length_ == 0 ? nullptr : buffer_.data();
    }

    // 처리 대기 중인 수신 데이터 길이.
    [[nodiscard]] std::size_t Length() const noexcept
    {
        return length_;
    }

    // async_read_some 에 넘길 쓰기 영역을 반환한다.
    // 남은 공간이 없으면 크기 0 버퍼를 반환.
    [[nodiscard]] boost::asio::mutable_buffer Prepare()
    {
        if (auto remaining = buffer_.size() - length_; remaining > 0)
        {
            return boost::asio::buffer(buffer_.data() + length_, remaining);
        }
        return boost::asio::mutable_buffer(nullptr, 0);
    }

    // 실제 수신된 바이트 수를 반영한다.
    void Commit(std::size_t bytesTransferred)
    {
        length_ += bytesTransferred;
    }

    // 처리 완료된 선두 바이트를 제거한다.
    void Consume(std::size_t bytesTransferred)
    {
        if (bytesTransferred == 0)
        {
            return;
        }

        if (length_ > bytesTransferred)
        {
            length_ -= bytesTransferred;
            std::memmove(buffer_.data(), buffer_.data() + bytesTransferred, length_);
        }
        else
        {
            length_ = 0;
        }
    }

    RecvBuffer(const RecvBuffer&) = delete;
    RecvBuffer& operator=(const RecvBuffer&) = delete;

private:
    std::vector<char> buffer_;
    std::size_t length_{};
};

} // namespace psh::network