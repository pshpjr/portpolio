#include "network/session.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/post.hpp>

#include <utility>

namespace psh::network
{

static constexpr UInt32 kMaxRecursionDepth = 64;
static thread_local UInt32 tl_ioDepth = 0;

struct DepthGuard
{
    DepthGuard() { ++tl_ioDepth; }
    ~DepthGuard() { --tl_ioDepth; }
    [[nodiscard]] bool ShouldPost() const { return tl_ioDepth > kMaxRecursionDepth; }
};

Session::Session(boost::asio::ip::tcp::socket socket, SessionID id,
                 OnPacketCallback onPacket, OnDisconnectCallback onDisconnect)
    : socket_(std::move(socket))
    , id_(id)
    , onPacket_(std::move(onPacket))
    , onDisconnect_(std::move(onDisconnect))
{
}

Session::~Session()
{
    Close();
}

void Session::Start()
{
    connected_.store(true, std::memory_order_release);
    DoRead();
}

void Session::Close()
{
    bool expected = true;
    if (!connected_.compare_exchange_strong(expected, false, std::memory_order_acq_rel))
    {
        return;
    }

    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);

    if (onDisconnect_)
    {
        onDisconnect_(*this);
    }
}

void Session::Send(std::shared_ptr<Packet> packet)
{
    if (!IsConnected() || !packet || packet->Size() == 0)
    {
        return;
    }

    {
        std::lock_guard lock(sendMutex_);
        sendQueue_.push_back(std::move(packet));
    }

    bool expected = false;
    if (sending_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        DoWrite();
    }
}

void Session::ProcessRecvBuffer()
{
    while (recvBuffer_.Length() >= kHeaderSize)
    {
        PacketHeader header{};
        std::memcpy(&header, recvBuffer_.Data(), kHeaderSize);

        if (header.magic != kPacketMagic)
        {
            Close();
            return;
        }

        if (header.length < kHeaderSize || header.length > kMaxPacketSize)
        {
            Close();
            return;
        }

        if (recvBuffer_.Length() < header.length)
        {
            break; // 아직 전체 패킷이 도착하지 않음
        }

        auto packet = Packet::FromRaw(recvBuffer_.Data(), header.length);
        recvBuffer_.Consume(header.length);

        if (onPacket_)
        {
            onPacket_(*this, std::move(packet));
        }
    }
}

void Session::DoRead()
{
    auto self = shared_from_this();
    auto buf = recvBuffer_.Prepare();

    if (buf.size() == 0)
    {
        Close();
        return;
    }

    socket_.async_read_some(
        boost::asio::buffer(buf),
        [self](boost::system::error_code ec, std::size_t bytesTransferred)
        {
            if (ec)
            {
                self->Close();
                return;
            }

            DepthGuard guard;

            self->recvBuffer_.Commit(bytesTransferred);
            self->ProcessRecvBuffer();

            if (guard.ShouldPost())
            {
                boost::asio::post(self->socket_.get_executor(),
                                  [self] { self->DoRead(); });
            }
            else
            {
                self->DoRead();
            }
        });
}

void Session::DoWrite()
{
    std::deque<std::shared_ptr<Packet>> pending;
    {
        std::lock_guard lock(sendMutex_);
        if (sendQueue_.empty())
        {
            sending_.store(false, std::memory_order_release);
            return;
        }
        pending.swap(sendQueue_);
    }

    auto buffers = std::vector<boost::asio::const_buffer>();
    buffers.reserve(pending.size());
    for (const auto& pkt : pending)
    {
        buffers.push_back(pkt->AsBuffer());
    }

    auto self = shared_from_this();
    boost::asio::async_write(
        socket_,
        buffers,
        [self, pending = std::move(pending)](boost::system::error_code ec, std::size_t /*bytesTransferred*/)
        {
            if (ec)
            {
                self->Close();
                return;
            }

            DepthGuard guard;
            if (guard.ShouldPost())
            {
                boost::asio::post(self->socket_.get_executor(),
                                  [self] { self->DoWrite(); });
            }
            else
            {
                self->DoWrite();
            }
        });
}

} // namespace psh::network
