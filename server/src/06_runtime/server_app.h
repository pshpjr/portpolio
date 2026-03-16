#pragma once

#include "05_network/aoi/aoi_grid.h"
#include "05_network/packet/packet_handler.h"
#include "05_network/session/session_manager.h"

namespace portpolio::runtime {

class ServerApp {
public:
    ServerApp(
        network::packet::PacketHandler& packetHandler,
        network::session::SessionManager& sessionManager,
        network::aoi::AoiGrid& aoiGrid);

    [[nodiscard]] bool boot() const;

private:
    network::packet::PacketHandler& m_packetHandler;
    network::session::SessionManager& m_sessionManager;
    network::aoi::AoiGrid& m_aoiGrid;
};

}  // namespace portpolio::runtime
