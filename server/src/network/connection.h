//
// Created by pshpjr on 26. 3. 29..
//

#ifndef PORTPOLIO_CONNECTION_H
#define PORTPOLIO_CONNECTION_H
#include "common/type_def.h"
#include "network/packet_handler/packet_handler.h"
enum class EnumConnectionState
{
    eNone,
    eConnected,
    eLogin,
    eCharacterSelected,
    eDisconnected
};

class Connection
{
    void Send();

private:
    AccountID accountId_;
    psh::network::HandlerMap packetHandlers_;
    EnumConnectionState state_{};
};

#endif // PORTPOLIO_CONNECTION_H
