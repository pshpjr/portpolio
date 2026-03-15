#pragma once

#include "02_config/server_config.h"
#include "01_types/entity_id.h"

namespace portpolio::core::combat {

class CombatSystem {
public:
    explicit CombatSystem(const config::ServerConfig& config);

    [[nodiscard]] types::EntityId lastAttacker() const;

private:
    const config::ServerConfig& m_config;
    types::EntityId m_lastAttacker;
};

}  // namespace portpolio::core::combat
