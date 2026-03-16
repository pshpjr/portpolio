#pragma once

#include "05_network/executor/network_executor.h"
#include "04_service/dungeon/dungeon_instance.h"

namespace portpolio::network::packet {

class PacketHandler {
public:
    PacketHandler(
        service::dungeon::DungeonInstance& dungeonInstance,
        executor::NetworkExecutor& networkExecutor);

    [[nodiscard]] bool canProcess() const;
    [[nodiscard]] bool usesContentExecutor() const;

private:
    service::dungeon::DungeonInstance& m_dungeonInstance;
    executor::NetworkExecutor& m_networkExecutor;
};

}  // namespace portpolio::network::packet
