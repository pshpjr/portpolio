#include "04_service/dungeon/dungeon_instance.h"

namespace portpolio::service::dungeon {

DungeonInstance::DungeonInstance(core::combat::CombatSystem& combatSystem)
    : m_combatSystem(combatSystem) {}

bool DungeonInstance::ready() const {
    return m_combatSystem.lastAttacker() != 0;
}

}  // namespace portpolio::service::dungeon
