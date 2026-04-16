#include "network/network_service.h"

#include <utility>

namespace psh::network
{

NetworkService::NetworkService(std::size_t threadCount)
    : threadCount_(threadCount > 0 ? threadCount : 1)
{
}

NetworkService::~NetworkService()
{
    Stop();
}

void NetworkService::Listen(UInt16 port,
                            Session::OnConnectCallback onConnect,
                            Session::OnPacketCallback onPacket,
                            Session::OnDisconnectCallback onDisconnect)
{
    auto wrappedDisconnect = [this, userCb = std::move(onDisconnect)](Session& session)
    {
        OnSessionDisconnected(session);
        if (userCb)
        {
            userCb(session);
        }
    };

    auto wrappedConnect = [this, userCb = std::move(onConnect)](std::shared_ptr<Session> session)
    {
        OnSessionCreated(session);
        if (userCb)
        {
            userCb(std::move(session));
        }
    };

    auto listener = std::make_unique<Listener>(
        ioc_, port, std::move(onPacket), std::move(wrappedDisconnect),
        std::move(wrappedConnect));

    listener->Start();
    listeners_.push_back(std::move(listener));
}

void NetworkService::Run()
{
    auto work = boost::asio::make_work_guard(ioc_);

    for (std::size_t i = 0; i < threadCount_; ++i)
    {
        threads_.emplace_back([this] { ioc_.run(); });
    }
}

void NetworkService::Stop()
{
    for (auto& listener : listeners_)
    {
        listener->Stop();
    }

    {
        std::lock_guard lock(sessionMutex_);
        for (auto& [id, session] : sessions_)
        {
            session->Close();
        }
        sessions_.clear();
    }

    ioc_.stop();

    for (auto& thread : threads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    threads_.clear();
}

void NetworkService::CloseSession(SessionID id)
{
    std::shared_ptr<Session> session;
    {
        std::lock_guard lock(sessionMutex_);
        auto it = sessions_.find(id);
        if (it == sessions_.end())
        {
            return;
        }
        session = it->second;
    }
    session->Close();
}

std::shared_ptr<Session> NetworkService::FindSession(SessionID id)
{
    std::lock_guard lock(sessionMutex_);
    auto it = sessions_.find(id);
    if (it != sessions_.end())
    {
        return it->second;
    }
    return nullptr;
}

void NetworkService::OnSessionCreated(std::shared_ptr<Session> session)
{
    std::lock_guard lock(sessionMutex_);
    sessions_.emplace(session->Id(), std::move(session));
}

void NetworkService::OnSessionDisconnected(Session& session)
{
    std::lock_guard lock(sessionMutex_);
    sessions_.erase(session.Id());
}

} // namespace psh::network
