#pragma once

#include "03_core/combat/combat_system.h"

namespace portpolio::service::dungeon {

class DungeonInstance {
public:
    explicit DungeonInstance(core::combat::CombatSystem& combatSystem);

    [[nodiscard]] bool ready() const;

private:
    core::combat::CombatSystem& m_combatSystem;
};

}  // namespace portpolio::service::dungeon
