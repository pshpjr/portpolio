#pragma once

#include "types/strong_id.h"

#include <filesystem>
#include <string>

namespace psh::config
{

// ---------------------------------------------------------------------------
// Config-domain strong types
// ---------------------------------------------------------------------------
using ServerName = psh::lib::types::StrongValue<struct ServerNameTag, std::string>;
using ShardId = psh::lib::types::StrongId<struct ShardIdTag, UInt32>;
using Port = psh::lib::types::StrongId<struct PortTag, UInt16>;
using DbHost = psh::lib::types::StrongValue<struct DbHostTag, std::string>;
using DbUser = psh::lib::types::StrongValue<struct DbUserTag, std::string>;
using DbPassword = psh::lib::types::StrongValue<struct DbPasswordTag, std::string>;
using RedisUrl = psh::lib::types::StrongValue<struct RedisUrlTag, std::string>;
using ServiceEndpoint = psh::lib::types::StrongValue<struct ServiceEndpointTag, std::string>;
using LogLevel = psh::lib::types::StrongValue<struct LogLevelTag, std::string>;

// ---------------------------------------------------------------------------
// ServerConfig
// Loaded from JSON (base), then overridden by environment variables.
//
// Required env vars (missing → log error + std::exit(1)):
//   PORT, DB_HOST, DB_USER, DB_PASSWORD, REDIS_URL
//
// Optional env vars (fall back to JSON or built-in default):
//   SERVER_NAME, SHARD_ID, SERVICE_ENDPOINT, FEATURE_FLAGS, LOG_LEVEL
// ---------------------------------------------------------------------------
struct ServerConfig
{
    ServerName serverName{std::string{"portpolio"}};
    ShardId shardId{1};
    Port port{8080};
    DbHost dbHost{std::string{}};
    DbUser dbUser{std::string{}};
    DbPassword dbPassword{std::string{}};
    RedisUrl redisUrl{std::string{}};
    ServiceEndpoint serviceEndpoint{std::string{}};
    std::string featureFlags{};
    LogLevel logLevel{std::string{"info"}};

    static ServerConfig Load(const std::filesystem::path& jsonPath);
};

} // namespace psh::config
