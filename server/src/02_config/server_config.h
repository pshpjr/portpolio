#pragma once

#include "01_types/entity_id.h"

#include <string>

namespace portpolio::config {

struct ServerConfig {
    std::string serverName{"portpolio"};
    types::EntityId shardId{1};
};

}  // namespace portpolio::config
