#include "pch.h"
#include "ConfigManager.h"

using web::json::value;

namespace
{
    std::wstring ReadString(value const& json, wchar_t const* key, std::wstring const& fallback)
    {
        if (!json.has_field(key) || !json.at(key).is_string())
        {
            return fallback;
        }
        return json.at(key).as_string();
    }

    bool ReadBool(value const& json, wchar_t const* key, bool fallback)
    {
        if (!json.has_field(key) || !json.at(key).is_boolean())
        {
            return fallback;
        }
        return json.at(key).as_bool();
    }

    int ReadInt(value const& json, wchar_t const* key, int fallback)
    {
        if (!json.has_field(key) || !json.at(key).is_integer())
        {
            return fallback;
        }
        return json.at(key).as_integer();
    }
}

BotConfig ConfigManager::Load()
{
    BotConfig config;
    auto path = ConfigPath();
    if (!std::filesystem::exists(path))
    {
        return config;
    }

    std::wifstream file(path);
    file.imbue(std::locale(".utf-8"));
    std::wstringstream buffer;
    buffer << file.rdbuf();

    try
    {
        auto json = value::parse(buffer.str());
        config.host = ReadString(json, L"host", config.host);
        config.port = ReadInt(json, L"port", config.port);
        config.onebotApiUrl = ReadString(json, L"onebot_api_url", config.onebotApiUrl);
        config.onebotAccessToken = ReadString(json, L"onebot_access_token", config.onebotAccessToken);
        config.aiBaseUrl = ReadString(json, L"ai_base_url", config.aiBaseUrl);
        config.aiApiKey = ReadString(json, L"ai_api_key", config.aiApiKey);
        config.aiModel = ReadString(json, L"ai_model", config.aiModel);
        config.systemPrompt = ReadString(json, L"system_prompt", config.systemPrompt);
        config.mode = ReadString(json, L"mode", config.mode);
        config.replyPrivate = ReadBool(json, L"reply_private", config.replyPrivate);
        config.replyGroup = ReadBool(json, L"reply_group", config.replyGroup);
        config.wakePrefix = ReadString(json, L"wake_prefix", config.wakePrefix);
        config.ownerJoinText = ReadString(json, L"owner_join_text", config.ownerJoinText);
    }
    catch (...)
    {
        return BotConfig{};
    }

    return config;
}

void ConfigManager::Save(BotConfig const& config)
{
    value json = value::object();
    json[L"host"] = value::string(config.host);
    json[L"port"] = value::number(config.port);
    json[L"onebot_api_url"] = value::string(config.onebotApiUrl);
    json[L"onebot_access_token"] = value::string(config.onebotAccessToken);
    json[L"ai_base_url"] = value::string(config.aiBaseUrl);
    json[L"ai_api_key"] = value::string(config.aiApiKey);
    json[L"ai_model"] = value::string(config.aiModel);
    json[L"system_prompt"] = value::string(config.systemPrompt);
    json[L"mode"] = value::string(config.mode);
    json[L"reply_private"] = value::boolean(config.replyPrivate);
    json[L"reply_group"] = value::boolean(config.replyGroup);
    json[L"wake_prefix"] = value::string(config.wakePrefix);
    json[L"owner_join_text"] = value::string(config.ownerJoinText);

    std::wofstream file(ConfigPath());
    file.imbue(std::locale(".utf-8"));
    file << json.serialize();
}

std::filesystem::path ConfigManager::ConfigPath()
{
    return std::filesystem::current_path() / L"config.json";
}
