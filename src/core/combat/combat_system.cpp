#include "core/combat/combat_system.h"

namespace portpolio::core::combat {

CombatSystem::CombatSystem(const config::ServerConfig& config)
    : m_config(config), m_lastAttacker(config.shardId) {}

types::EntityId CombatSystem::lastAttacker() const {
    return m_lastAttacker;
}

}  // namespace portpolio::core::combat
