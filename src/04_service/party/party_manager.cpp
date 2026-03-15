#include "04_service/party/party_manager.h"

namespace portpolio::service::party {

PartyManager::PartyManager(core::inventory::InventoryManager& inventoryManager)
    : m_inventoryManager(inventoryManager) {}

bool PartyManager::canEnterDungeon() const {
    return m_inventoryManager.canPickup(1);
}

}  // namespace portpolio::service::party
