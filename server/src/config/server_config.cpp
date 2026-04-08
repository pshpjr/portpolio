#include "config/server_config.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <fstream>

namespace psh::config {

namespace {

std::optional<std::string> env(const char* key)
{
    if (const char* v = std::getenv(key)) return std::string{v};
    return std::nullopt;
}

// Exits if the env var is missing. Logs both the missing var and a fix hint.
std::string requireEnv(const char* key)
{
    if (const char* v = std::getenv(key)) return v;
    spdlog::error("[config] Required environment variable '{}' is not set.", key);
    spdlog::error("[config] Set it in your environment or copy .env.example to .env and fill in the values.");
    std::exit(1);
}

// Logs when an env var overrides a value that was already set from JSON.
template <typename T>
void logOverride(const char* key, const T& before, const T& after)
{
    spdlog::info("[config] '{}' overridden by env: '{}' -> '{}'", key,
                 psh::lib::types::ValueOf(before),
                 psh::lib::types::ValueOf(after));
}

void logOverride(const char* key, const std::string& before, const std::string& after)
{
    spdlog::info("[config] '{}' overridden by env: '{}' -> '{}'", key, before, after);
}

}  // namespace

ServerConfig ServerConfig::Load(const std::filesystem::path& jsonPath)
{
    ServerConfig cfg;

    // 1. JSON file (base values — all optional)
    if (std::ifstream f{jsonPath}; f.is_open()) {
        const auto j = nlohmann::json::parse(f);
        if (j.contains("server_name"))      cfg.serverName      = ServerName     {j["server_name"].get<std::string>()};
        if (j.contains("shard_id"))         cfg.shardId         = ShardId        {j["shard_id"].get<UInt32>()};
        if (j.contains("port"))             cfg.port            = Port           {j["port"].get<UInt16>()};
        if (j.contains("db_host"))          cfg.dbHost          = DbHost         {j["db_host"].get<std::string>()};
        if (j.contains("db_user"))          cfg.dbUser          = DbUser         {j["db_user"].get<std::string>()};
        if (j.contains("db_password"))      cfg.dbPassword      = DbPassword     {j["db_password"].get<std::string>()};
        if (j.contains("redis_url"))        cfg.redisUrl        = RedisUrl       {j["redis_url"].get<std::string>()};
        if (j.contains("service_endpoint")) cfg.serviceEndpoint = ServiceEndpoint{j["service_endpoint"].get<std::string>()};
        if (j.contains("feature_flags"))    cfg.featureFlags    = j["feature_flags"].get<std::string>();
        if (j.contains("log_level"))        cfg.logLevel        = LogLevel       {j["log_level"].get<std::string>()};
    }

    // 2. Required env vars — missing → error message + exit(1)
    cfg.port       = Port      {static_cast<UInt16>(std::stoul(requireEnv("PORT")))};
    cfg.dbHost     = DbHost    {requireEnv("DB_HOST")};
    cfg.dbUser     = DbUser    {requireEnv("DB_USER")};
    cfg.dbPassword = DbPassword{requireEnv("DB_PASSWORD")};
    cfg.redisUrl   = RedisUrl  {requireEnv("REDIS_URL")};

    // 3. Optional env var overrides — log when a JSON value is replaced
    if (auto v = env("SERVER_NAME")) {
        auto next = ServerName{*v};
        if (next != cfg.serverName) logOverride("SERVER_NAME", cfg.serverName, next);
        cfg.serverName = next;
    }
    if (auto v = env("SHARD_ID")) {
        auto next = ShardId{static_cast<UInt32>(std::stoul(*v))};
        if (next != cfg.shardId) logOverride("SHARD_ID", cfg.shardId, next);
        cfg.shardId = next;
    }
    if (auto v = env("SERVICE_ENDPOINT")) {
        auto next = ServiceEndpoint{*v};
        if (next != cfg.serviceEndpoint) logOverride("SERVICE_ENDPOINT", cfg.serviceEndpoint, next);
        cfg.serviceEndpoint = next;
    }
    if (auto v = env("FEATURE_FLAGS")) {
        if (*v != cfg.featureFlags) logOverride("FEATURE_FLAGS", cfg.featureFlags, *v);
        cfg.featureFlags = *v;
    }
    if (auto v = env("LOG_LEVEL")) {
        auto next = LogLevel{*v};
        if (next != cfg.logLevel) logOverride("LOG_LEVEL", cfg.logLevel, next);
        cfg.logLevel = next;
    }

    return cfg;
}

}  // namespace psh::config
