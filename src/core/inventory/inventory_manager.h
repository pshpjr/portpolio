#pragma once

#include "types/entity_id.h"

namespace portpolio::core::inventory {

class InventoryManager {
public:
    [[nodiscard]] bool canPickup(types::EntityId owner) const;
};

}  // namespace portpolio::core::inventory
