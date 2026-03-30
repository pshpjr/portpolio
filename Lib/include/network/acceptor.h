//
// Created by pshpjr on 26. 3. 30..
//

#ifndef PORTPOLIO_ACCEPTOR_H
#define PORTPOLIO_ACCEPTOR_H
#include "io_handler.h"

#include <cstdint>
#include <mutex>
#include <memory>
#include <unordered_set>
#include <thread>
#include <vector>

namespace psh::lib::network
{
class Session;

class Acceptor : public std::enable_shared_from_this<Acceptor>, public IoHandler
{
public:
    Acceptor(uint16_t port);
    ~Acceptor();
    void Listen();
    void DisconnectAll();
    void Stop();

private:
    void StartAccept();
    void OnSessionClosed(Session* session) override;

    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::mutex sessionMutex_{};
    std::unordered_set<std::shared_ptr<Session>> sessions_{};
};
}

#endif //PORTPOLIO_ACCEPTOR_H
