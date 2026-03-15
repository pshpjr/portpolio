#include "network/aoi/aoi_grid.h"

namespace portpolio::network::aoi {

AoiGrid::AoiGrid(service::matchmaking::MatchmakingService& matchmakingService)
    : m_matchmakingService(matchmakingService) {}

bool AoiGrid::initialized() const {
    return m_matchmakingService.hasMinimumPower();
}

}  // namespace portpolio::network::aoi
