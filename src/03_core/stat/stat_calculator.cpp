#include "03_core/stat/stat_calculator.h"

namespace portpolio::core::stat {

int StatCalculator::baseHealth(types::EntityId entityId) const {
    return entityId == 0 ? 0 : 100;
}

}  // namespace portpolio::core::stat
