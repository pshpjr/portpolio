//
// Created by pshpjr on 26. 3. 30..
//
#include "acceptor.h"

#include "network/session.h"

#include <utility>

namespace psh::lib::network
{
using namespace boost::asio;

Acceptor::Acceptor(uint16_t port)
    : endpoint_(ip::tcp::endpoint(ip::tcp::v4(), port))
    , acceptor_(ioContext_, endpoint_)
{
}
Acceptor::~Acceptor()
{
    Stop();
}

void Acceptor::DisconnectAll()
{
    std::vector<std::shared_ptr<Session>> sessions;
    {
        std::scoped_lock lock(sessionMutex_);
        sessions.assign(sessions_.begin(), sessions_.end());
        sessions_.clear();
    }

    for (const std::shared_ptr<Session>& session : sessions)
    {
        if (session)
        {
            session->Close();
        }
    }
}

void Acceptor::Stop()
{
    boost::system::error_code ec;
    acceptor_.cancel(ec);
    acceptor_.close(ec);

    DisconnectAll();

    guard_.reset();
    ioContext_.stop();
    threads_.clear();
}

void Acceptor::Listen()
{
    StartAccept();
    threads_.emplace_back([this, self = shared_from_this()]() { ioContext_.run(); });
    ioContext_.run();
}

void Acceptor::StartAccept()
{
    if (!acceptor_.is_open())
    {
        return;
    }

    acceptor_.async_accept(
        [this](boost::system::error_code ec, ip::tcp::socket socket)
        {
            if (ec)
            {
                //TODO: 에러 로깅
            }
            else
            {
                socket.set_option(ip::tcp::no_delay(true));
                auto session = std::make_shared<Session>(ioContext_, std::move(socket), *this);

                {
                    std::scoped_lock lock(sessionMutex_);
                    sessions_.insert(session);
                }

                NotifyConnect(session);

                session->StartRecv();
            }

            if (acceptor_.is_open())
            {
                StartAccept();
            }
        });
}

void Acceptor::OnSessionClosed(Session* session)
{
    std::scoped_lock lock(sessionMutex_);

    for (auto iter = sessions_.begin(); iter != sessions_.end(); ++iter)
    {
        if (iter->get() == session)
        {
            sessions_.erase(iter);
            break;
        }
    }
}

} // namespace psh::lib::network
