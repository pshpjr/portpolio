#pragma once

#include "service/party/party_manager.h"

namespace portpolio::network::session {

class SessionManager {
public:
    explicit SessionManager(service::party::PartyManager& partyManager);

    [[nodiscard]] bool acceptsConnections() const;

private:
    service::party::PartyManager& m_partyManager;
};

}  // namespace portpolio::network::session
