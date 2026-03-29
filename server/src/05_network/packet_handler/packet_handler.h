//
// Created by pshpjr on 26. 3. 29..
//

#ifndef PORTPOLIO_PACKET_HANDLER_H
#define PORTPOLIO_PACKET_HANDLER_H
#include "01_types/type_def.h"

namespace psh::network
{
//
using HandlerFunction = void(*)(short, char*);
using HandlerMap = HashMap<short, HandlerFunction>;



inline void OnRecvTest(short uid, char* data)
{
    return;
}

static HandlerMap gConnectionHandlerMap =
{
    {1,OnRecvTest}
};

}

#endif //PORTPOLIO_PACKET_HANDLER_H
