#include "05_network/packet/packet_handler.h"

namespace portpolio::network::packet {

PacketHandler::PacketHandler(service::dungeon::DungeonInstance& dungeonInstance)
    : m_dungeonInstance(dungeonInstance) {}

bool PacketHandler::canProcess() const {
    return m_dungeonInstance.ready();
}

}  // namespace portpolio::network::packet
