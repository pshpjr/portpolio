#pragma once

#include "03_core/stat/stat_calculator.h"

namespace portpolio::service::matchmaking {

class MatchmakingService {
public:
    explicit MatchmakingService(core::stat::StatCalculator& statCalculator);

    [[nodiscard]] bool hasMinimumPower() const;

private:
    core::stat::StatCalculator& m_statCalculator;
};

}  // namespace portpolio::service::matchmaking
