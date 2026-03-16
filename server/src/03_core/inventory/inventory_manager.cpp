#include "03_core/inventory/inventory_manager.h"

namespace portpolio::core::inventory {

bool InventoryManager::canPickup(types::EntityId owner) const {
    return owner != 0;
}

}  // namespace portpolio::core::inventory
