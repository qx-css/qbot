#pragma once

#include "BotConfig.h"

class BotEngine
{
public:
    using LogHandler = std::function<void(std::wstring const&)>;

    explicit BotEngine(LogHandler logHandler);
    ~BotEngine();

    void Configure(BotConfig config);
    void Start();
    void Stop();
    bool IsRunning() const;
    void TestAi();

private:
    void HandleEvent(web::json::value const& event);
    void HandleGroupIncrease(web::json::value const& event);
    void HandleMessage(web::json::value event);
    std::wstring AskAi(std::wstring const& userText);
    void SendGroupMessage(int64_t groupId, std::wstring const& message);
    void SendPrivateMessage(int64_t userId, std::wstring const& message);
    web::json::value PostJson(std::wstring const& url, web::json::value const& payload, std::wstring const& bearerToken);
    void Log(std::wstring const& message) const;
    static std::wstring Now();
    static bool HasPrefix(std::wstring const& text, std::wstring const& prefix);
    static int64_t ReadInt64(web::json::value const& json, wchar_t const* key);
    static std::wstring ReadString(web::json::value const& json, wchar_t const* key);

    mutable std::mutex m_mutex;
    BotConfig m_config;
    LogHandler m_logHandler;
    std::unique_ptr<web::http::experimental::listener::http_listener> m_listener;
    bool m_running = false;
};
