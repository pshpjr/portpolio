#include "05_network/session/session_manager.h"

namespace portpolio::network::session {

SessionManager::SessionManager(
    service::party::PartyManager& partyManager,
    executor::NetworkExecutor& networkExecutor)
    : m_partyManager(partyManager), m_networkExecutor(networkExecutor) {}

bool SessionManager::acceptsConnections() const {
    return m_partyManager.canEnterDungeon();
}

bool SessionManager::usesSeparatedExecutors() const {
    return m_networkExecutor.separated();
}

}  // namespace portpolio::network::session
