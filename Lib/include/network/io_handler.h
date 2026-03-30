//
// Created by pshpjr on 26. 3. 30..
//

#ifndef PORTPOLIO_IO_HANDLER_H
#define PORTPOLIO_IO_HANDLER_H

#include <memory>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace psh::lib::network
{
class Session;
class IoHandler
{
public:
    using connect_callback_t = void(*)(std::shared_ptr<Session>&);
    using disconnect_callback_t = void(*)(std::shared_ptr<Session>&);
    using recv_callback_t = void(*)(char*,int);

    IoHandler() : guard_(boost::asio::make_work_guard(ioContext_)){}

    void SetConnectCallback(connect_callback_t callback) { connectCallback_ = callback; }
    void SetDisconnectCallback(disconnect_callback_t callback) { disconnectCallback_ = callback; }
    void SetRecvCallback(recv_callback_t callback) { recvCallback_ = callback; }
    void NotifyConnect(std::shared_ptr<Session>& session)
    {
        if (connectCallback_)
        {
            connectCallback_(session);
        }
    }
    void NotifyDisconnect(std::shared_ptr<Session>& session)
    {
        if (disconnectCallback_)
        {
            disconnectCallback_(session);
        }
    }
    void NotifyRecv(char* data, int length)
    {
        if (recvCallback_)
        {
            recvCallback_(data, length);
        }
    }
    virtual void OnSessionClosed(Session* session) { (void)session; }

protected:
    using io_context = boost::asio::io_context;
    using guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    io_context ioContext_;
    guard_t guard_;
    std::vector<std::jthread> threads_{};

    connect_callback_t      connectCallback_{nullptr};
    disconnect_callback_t   disconnectCallback_{nullptr};
    recv_callback_t         recvCallback_{nullptr};
};
}




#endif //PORTPOLIO_IO_HANDLER_H
