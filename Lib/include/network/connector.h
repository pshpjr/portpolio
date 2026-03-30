//
// Created by pshpjr on 26. 3. 30..
//

#ifndef PORTPOLIO_CONNECTOR_H
#define PORTPOLIO_CONNECTOR_H

#include <memory>
#include <string_view>

#include <boost/asio/ip/tcp.hpp>

#include "io_handler.h"

namespace psh::lib::network
{
class Connector : public std::enable_shared_from_this<Connector>, public IoHandler
{
public:
    Connector() = default;

    void Connect(std::string_view address, std::string_view port);

private:
    boost::asio::ip::tcp::endpoint endpoint_;
};
} // namespace psh::lib::network

#endif //PORTPOLIO_CONNECTOR_H
