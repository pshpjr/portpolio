#pragma once

#include "core/inventory/inventory_manager.h"

namespace portpolio::service::party {

class PartyManager {
public:
    explicit PartyManager(core::inventory::InventoryManager& inventoryManager);

    [[nodiscard]] bool canEnterDungeon() const;

private:
    core::inventory::InventoryManager& m_inventoryManager;
};

}  // namespace portpolio::service::party
