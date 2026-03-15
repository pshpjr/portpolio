#include "config/server_config.h"
#include "core/combat/combat_system.h"
#include "core/inventory/inventory_manager.h"
#include "core/stat/stat_calculator.h"
#include "network/aoi/aoi_grid.h"
#include "network/packet/packet_handler.h"
#include "network/session/session_manager.h"
#include "runtime/server_app.h"
#include "service/dungeon/dungeon_instance.h"
#include "service/matchmaking/matchmaking_service.h"
#include "service/party/party_manager.h"

#include <gtest/gtest.h>

TEST(ScaffoldSmokeTest, BootstrapsServerApp) {
    portpolio::config::ServerConfig config{};
    portpolio::core::combat::CombatSystem combatSystem(config);
    portpolio::core::inventory::InventoryManager inventoryManager;
    portpolio::core::stat::StatCalculator statCalculator;
    portpolio::service::dungeon::DungeonInstance dungeonInstance(combatSystem);
    portpolio::service::party::PartyManager partyManager(inventoryManager);
    portpolio::service::matchmaking::MatchmakingService matchmakingService(
        statCalculator);
    portpolio::network::packet::PacketHandler packetHandler(dungeonInstance);
    portpolio::network::session::SessionManager sessionManager(partyManager);
    portpolio::network::aoi::AoiGrid aoiGrid(matchmakingService);
    portpolio::runtime::ServerApp app(packetHandler, sessionManager, aoiGrid);

    EXPECT_TRUE(app.boot());
}
