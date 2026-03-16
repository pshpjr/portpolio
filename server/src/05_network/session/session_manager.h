#pragma once

#include "05_network/executor/network_executor.h"
#include "04_service/party/party_manager.h"

namespace portpolio::network::session {

class SessionManager {
public:
    SessionManager(
        service::party::PartyManager& partyManager,
        executor::NetworkExecutor& networkExecutor);

    [[nodiscard]] bool acceptsConnections() const;
    [[nodiscard]] bool usesSeparatedExecutors() const;

private:
    service::party::PartyManager& m_partyManager;
    executor::NetworkExecutor& m_networkExecutor;
};

}  // namespace portpolio::network::session
