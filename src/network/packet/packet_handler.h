#pragma once

#include "service/dungeon/dungeon_instance.h"

namespace portpolio::network::packet {

class PacketHandler {
public:
    explicit PacketHandler(service::dungeon::DungeonInstance& dungeonInstance);

    [[nodiscard]] bool canProcess() const;

private:
    service::dungeon::DungeonInstance& m_dungeonInstance;
};

}  // namespace portpolio::network::packet
