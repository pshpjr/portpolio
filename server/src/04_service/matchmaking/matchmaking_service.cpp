#include "04_service/matchmaking/matchmaking_service.h"

namespace portpolio::service::matchmaking {

MatchmakingService::MatchmakingService(core::stat::StatCalculator& statCalculator)
    : m_statCalculator(statCalculator) {}

bool MatchmakingService::hasMinimumPower() const {
    return m_statCalculator.baseHealth(1) >= 100;
}

}  // namespace portpolio::service::matchmaking
