#pragma once

#include "04_service/matchmaking/matchmaking_service.h"

namespace portpolio::network::aoi {

class AoiGrid {
public:
    explicit AoiGrid(service::matchmaking::MatchmakingService& matchmakingService);

    [[nodiscard]] bool initialized() const;

private:
    service::matchmaking::MatchmakingService& m_matchmakingService;
};

}  // namespace portpolio::network::aoi
