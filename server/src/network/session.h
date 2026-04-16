#pragma once

#include "network/packet.h"
#include "network/recv_buffer.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>

#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace psh::network
{

using SessionID = UInt64;

class Session : public std::enable_shared_from_this<Session>
{
public:
    using OnConnectCallback = std::function<void(std::shared_ptr<Session>)>;
    using OnPacketCallback = std::function<void(Session&, std::shared_ptr<Packet> packet)>;
    using OnDisconnectCallback = std::function<void(Session&)>;

    Session(boost::asio::ip::tcp::socket socket, SessionID id,
            OnPacketCallback onPacket, OnDisconnectCallback onDisconnect);

    ~Session();

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    void Start();
    void Close();
    void Send(std::shared_ptr<Packet> packet);

    [[nodiscard]] SessionID Id() const noexcept { return id_; }
    [[nodiscard]] bool IsConnected() const noexcept { return connected_.load(std::memory_order_acquire); }

private:
    void DoRead();
    void ProcessRecvBuffer();
    void DoWrite();

    boost::asio::ip::tcp::socket socket_;
    SessionID id_;
    std::atomic<bool> connected_{false};

    RecvBuffer recvBuffer_;

    std::mutex sendMutex_;
    std::deque<std::shared_ptr<Packet>> sendQueue_;
    std::atomic<bool> sending_{false};

    OnPacketCallback onPacket_;
    OnDisconnectCallback onDisconnect_;
};

} // namespace psh::network
