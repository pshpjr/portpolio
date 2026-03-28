#pragma once

#include "01_types/entity_id.h"

#include <memory>

namespace psh::core {

using types::EntityId;
class Item;

class Inventory
{
public:



private:
    std::unordered_map<EntityId, std::shared_ptr<Item>> m_repository;
};


}  // namespace psh::core
