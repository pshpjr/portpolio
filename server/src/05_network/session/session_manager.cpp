#include "05_network/session/session_manager.h"

namespace portpolio::network::session {

SessionManager::SessionManager(service::party::PartyManager& partyManager)
    : m_partyManager(partyManager) {}

bool SessionManager::acceptsConnections() const {
    return m_partyManager.canEnterDungeon();
}

}  // namespace portpolio::network::session
