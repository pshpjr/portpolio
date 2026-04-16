#pragma once

#include "network/listener.h"
#include "network/session.h"

#include <boost/asio/io_context.hpp>

#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace psh::network
{

class NetworkService
{
public:
    explicit NetworkService(std::size_t threadCount = 1);
    ~NetworkService();

    NetworkService(const NetworkService&) = delete;
    NetworkService& operator=(const NetworkService&) = delete;

    void Listen(UInt16 port,
                Session::OnConnectCallback onConnect,
                Session::OnPacketCallback onPacket,
                Session::OnDisconnectCallback onDisconnect);

    void Run();
    void Stop();

    void CloseSession(SessionID id);
    std::shared_ptr<Session> FindSession(SessionID id);

    boost::asio::io_context& GetIoContext() noexcept { return ioc_; }

private:
    void OnSessionCreated(std::shared_ptr<Session> session);
    void OnSessionDisconnected(Session& session);

    boost::asio::io_context ioc_;
    std::size_t threadCount_;
    std::vector<std::thread> threads_;
    std::vector<std::unique_ptr<Listener>> listeners_;

    std::mutex sessionMutex_;
    std::unordered_map<SessionID, std::shared_ptr<Session>> sessions_;
};

} // namespace psh::network
