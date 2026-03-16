#include "02_config/server_config.h"
#include "03_core/combat/combat_system.h"
#include "03_core/inventory/inventory_manager.h"
#include "03_core/stat/stat_calculator.h"
#include "04_service/dungeon/dungeon_instance.h"
#include "04_service/matchmaking/matchmaking_service.h"
#include "04_service/party/party_manager.h"
#include "05_network/aoi/aoi_grid.h"
#include "05_network/packet/packet_handler.h"
#include "05_network/session/session_manager.h"
#include "06_runtime/server_app.h"


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
