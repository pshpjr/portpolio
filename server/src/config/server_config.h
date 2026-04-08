#pragma once

#include "common/entity_id.h"

#include <string>

namespace psh::config {

struct ServerConfig {
    std::string serverName{"portpolio"};
    EntityId shardId{1};
};

}  // namespace portpolio::config
