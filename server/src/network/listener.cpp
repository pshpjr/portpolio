#include "network/listener.h"

#include <utility>

namespace psh::network
{

Listener::Listener(boost::asio::io_context& ioc, UInt16 port,
                   Session::OnPacketCallback onPacket,
                   Session::OnDisconnectCallback onDisconnect,
                   Session::OnConnectCallback onSessionCreated)
    : ioc_(ioc)
    , acceptor_(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , onPacket_(std::move(onPacket))
    , onDisconnect_(std::move(onDisconnect))
    , onSessionCreated_(std::move(onSessionCreated))
{
}

void Listener::Start()
{
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    DoAccept();
}

void Listener::Stop()
{
    boost::system::error_code ec;
    acceptor_.close(ec);
}

void Listener::DoAccept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if (ec)
            {
                return;
            }

            auto id = nextSessionId_.fetch_add(1, std::memory_order_relaxed);
            auto session = std::make_shared<Session>(
                std::move(socket), id, onPacket_, onDisconnect_);

            if (onSessionCreated_)
            {
                onSessionCreated_(session);
            }

            session->Start();
            DoAccept();
        });
}

} // namespace psh::network
