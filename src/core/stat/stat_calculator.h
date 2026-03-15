#pragma once

#include "types/entity_id.h"

namespace portpolio::core::stat {

class StatCalculator {
public:
    [[nodiscard]] int baseHealth(types::EntityId entityId) const;
};

}  // namespace portpolio::core::stat
