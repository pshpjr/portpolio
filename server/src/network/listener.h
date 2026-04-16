#pragma once

#include "network/session.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <atomic>
#include <functional>

namespace psh::network
{

class Listener
{
public:
    Listener(boost::asio::io_context& ioc, UInt16 port,
             Session::OnPacketCallback onPacket,
             Session::OnDisconnectCallback onDisconnect,
             Session::OnConnectCallback onSessionCreated);

    void Start();
    void Stop();

private:
    void DoAccept();

    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::atomic<SessionID> nextSessionId_{1};

    Session::OnPacketCallback onPacket_;
    Session::OnDisconnectCallback onDisconnect_;
    Session::OnConnectCallback onSessionCreated_;
};

} // namespace psh::network
