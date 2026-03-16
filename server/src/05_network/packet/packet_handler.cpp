#include "05_network/packet/packet_handler.h"

namespace portpolio::network::packet {

PacketHandler::PacketHandler(
    service::dungeon::DungeonInstance& dungeonInstance,
    executor::NetworkExecutor& networkExecutor)
    : m_dungeonInstance(dungeonInstance), m_networkExecutor(networkExecutor) {}

bool PacketHandler::canProcess() const {
    return m_dungeonInstance.ready();
}

bool PacketHandler::usesContentExecutor() const {
    return m_networkExecutor.content().name() == "content";
}

}  // namespace portpolio::network::packet
