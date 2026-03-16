#include "06_runtime/server_app.h"

namespace portpolio::runtime {

ServerApp::ServerApp(
    network::packet::PacketHandler& packetHandler,
    network::session::SessionManager& sessionManager,
    network::aoi::AoiGrid& aoiGrid)
    : m_packetHandler(packetHandler),
      m_sessionManager(sessionManager),
      m_aoiGrid(aoiGrid) {}

bool ServerApp::boot() const {
    return m_packetHandler.canProcess() && m_sessionManager.acceptsConnections() &&
           m_aoiGrid.initialized();
}

}  // namespace portpolio::runtime
