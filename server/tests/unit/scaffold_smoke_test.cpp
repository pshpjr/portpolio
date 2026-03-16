#include "02_config/server_config.h"
#include "03_core/combat/combat_system.h"
#include "03_core/inventory/inventory_manager.h"
#include "03_core/stat/stat_calculator.h"
#include "04_service/dungeon/dungeon_instance.h"
#include "04_service/matchmaking/matchmaking_service.h"
#include "04_service/party/party_manager.h"
#include "05_network/aoi/aoi_grid.h"
#include "05_network/executor/network_executor.h"
#include "05_network/packet/packet_handler.h"
#include "05_network/session/session_manager.h"
#include "06_runtime/server_app.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>

#include <atomic>

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
    boost::asio::io_context ioContext;
    boost::asio::thread_pool contentPool(1);
    portpolio::network::executor::NetworkExecutor networkExecutor(
        ioContext.get_executor(),
        contentPool.get_executor());
    portpolio::network::packet::PacketHandler packetHandler(
        dungeonInstance,
        networkExecutor);
    portpolio::network::session::SessionManager sessionManager(
        partyManager,
        networkExecutor);
    portpolio::network::aoi::AoiGrid aoiGrid(matchmakingService);
    portpolio::runtime::ServerApp app(
        networkExecutor,
        packetHandler,
        sessionManager,
        aoiGrid);

    EXPECT_TRUE(app.boot());
}

TEST(NetworkExecutorTest, DispatchesIoAndContentSeparately) {
    boost::asio::io_context ioContext;
    boost::asio::thread_pool contentPool(1);
    portpolio::network::executor::NetworkExecutor networkExecutor(
        ioContext.get_executor(),
        contentPool.get_executor());
    std::atomic_int executed{0};

    networkExecutor.dispatchIo([&executed]() { ++executed; });
    networkExecutor.dispatchContent([&executed]() { ++executed; });

    ioContext.run();
    contentPool.join();

    EXPECT_TRUE(networkExecutor.separated());
    EXPECT_EQ(executed.load(), 2);
}
