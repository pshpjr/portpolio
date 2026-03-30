//
// Created by pshpjr on 26. 3. 30..
//

#ifndef PORTPOLIO_SESSION_H
#define PORTPOLIO_SESSION_H
#include <array>
#include <atomic>
#include <cstddef>
#include <memory>
#include <vector>

#include <boost/asio/socket_base.hpp>

#include <moodycamel/concurrentqueue.h>

#include "const_def.h"
#include "io_handler.h"

#include <boost/asio/ip/tcp.hpp>

namespace boost::asio
{
class io_context;
}

namespace psh::lib::network
{
class Session : public std::enable_shared_from_this<Session>
{
    friend class Acceptor;
public:
    Session(boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket&& socket, IoHandler& ioHandler);

    void Send(const char* data, size_t length);
    void SendClose();
    void Close();

    auto GetIoContext() const -> boost::asio::io_context&;
    auto GetEndpoint() const -> boost::asio::ip::tcp::endpoint;

private:
    void StartRecv();
    void TryStartSend();
    void StartSend();
    void FinishClose();

    std::array<char, kMaxPacketSize> buffer_{};
    size_t bufferBegin_{};
    size_t bufferEnd_{};
    moodycamel::ConcurrentQueue<std::shared_ptr<std::vector<char>>> sendQueue_{};
    std::shared_ptr<std::vector<char>> currentSendBuffer_{};
    std::atomic<size_t> pendingSendCount_{0};
    std::atomic<bool>   isSending_{false};
    std::atomic<bool>   isClosed_{false};
    std::atomic<bool>   needSendClose_{false};

    boost::asio::io_context& ioContext_;
    boost::asio::ip::tcp::socket socket_;

    IoHandler& ioHandler_;
};

} // namespace psh::lib::network

#endif // PORTPOLIO_SESSION_H
