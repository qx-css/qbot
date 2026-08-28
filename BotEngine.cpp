#include "pch.h"
#include "BotEngine.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;

BotEngine::BotEngine(LogHandler logHandler) : m_logHandler(std::move(logHandler))
{
}

BotEngine::~BotEngine()
{
    Stop();
}

void BotEngine::Configure(BotConfig config)
{
    std::scoped_lock lock(m_mutex);
    m_config = std::move(config);
}

void BotEngine::Start()
{
    BotConfig config;
    {
        std::scoped_lock lock(m_mutex);
        if (m_running)
        {
            return;
        }
        config = m_config;
    }

    uri_builder builder;
    builder.set_scheme(L"http");
    builder.set_host(config.host);
    builder.set_port(config.port);

    auto listener = std::make_unique<http_listener>(builder.to_uri());
    listener->support(methods::POST, [this](http_request request)
    {
        request.extract_json().then([this, request](pplx::task<json::value> task) mutable
        {
            try
            {
                HandleEvent(task.get());
                json::value response = json::value::object();
                response[L"status"] = json::value::string(L"ok");
                request.reply(status_codes::OK, response);
            }
            catch (std::exception const& ex)
            {
                Log(std::wstring(L"\u4e8b\u4ef6\u5904\u7406\u5931\u8d25\uff1a") + winrt::to_hstring(ex.what()).c_str());
                request.reply(status_codes::InternalError, json::value::string(L"failed"));
            }
        });
    });
    listener->support(methods::GET, [](http_request request)
    {
        json::value response = json::value::object();
        response[L"status"] = json::value::string(L"ok");
        request.reply(status_codes::OK, response);
    });

    listener->open().wait();

    {
        std::scoped_lock lock(m_mutex);
        m_listener = std::move(listener);
        m_running = true;
    }
    Log(L"\u5df2\u542f\u52a8\uff1ahttp://" + config.host + L":" + std::to_wstring(config.port));
}

void BotEngine::Stop()
{
    std::unique_ptr<http_listener> listener;
    {
        std::scoped_lock lock(m_mutex);
        if (!m_running)
        {
            return;
        }
        listener = std::move(m_listener);
        m_running = false;
    }

    if (listener)
    {
        listener->close().wait();
    }
    Log(L"\u5df2\u505c\u6b62");
}

bool BotEngine::IsRunning() const
{
    std::scoped_lock lock(m_mutex);
    return m_running;
}

void BotEngine::TestAi()
{
    std::thread([this]
    {
        auto reply = AskAi(L"\u8bf7\u7528\u4e00\u53e5\u8bdd\u4ecb\u7ecd\u4f60\u81ea\u5df1\u3002");
        Log(reply.empty() ? L"AI \u6d4b\u8bd5\u5931\u8d25" : L"AI \u6d4b\u8bd5\u56de\u590d\uff1a" + reply);
    }).detach();
}

void BotEngine::HandleEvent(json::value const& event)
{
    auto postType = ReadString(event, L"post_type");
    if (postType == L"notice" && ReadString(event, L"notice_type") == L"group_increase")
    {
        HandleGroupIncrease(event);
        return;
    }
    if (postType == L"message")
    {
        std::thread([this, event]
        {
            HandleMessage(event);
        }).detach();
    }
}

void BotEngine::HandleGroupIncrease(json::value const& event)
{
    auto groupId = ReadInt64(event, L"group_id");
    auto userId = ReadInt64(event, L"user_id");
    auto selfId = ReadInt64(event, L"self_id");
    if (groupId > 0 && userId > 0 && userId == selfId)
    {
        BotConfig config;
        {
            std::scoped_lock lock(m_mutex);
            config = m_config;
        }
        Log(L"\u673a\u5668\u4eba\u52a0\u5165\u7fa4 " + std::to_wstring(groupId) + L"\uff0c\u53d1\u9001\u5165\u7fa4\u63d0\u793a");
        SendGroupMessage(groupId, config.ownerJoinText);
    }
}

void BotEngine::HandleMessage(json::value event)
{
    BotConfig config;
    {
        std::scoped_lock lock(m_mutex);
        config = m_config;
    }

    auto messageType = ReadString(event, L"message_type");
    auto rawMessage = ReadString(event, L"raw_message");
    if (rawMessage.empty())
    {
        return;
    }
    if (messageType == L"group" && !config.replyGroup)
    {
        return;
    }
    if (messageType == L"private" && !config.replyPrivate)
    {
        return;
    }
    if (!config.wakePrefix.empty() && !HasPrefix(rawMessage, config.wakePrefix))
    {
        return;
    }
    if (config.mode != L"AI")
    {
        Log(L"\u5f53\u524d\u4e0d\u662f AI \u6a21\u5f0f\uff0c\u5df2\u5ffd\u7565\u6d88\u606f");
        return;
    }

    auto userText = config.wakePrefix.empty() ? rawMessage : rawMessage.substr(config.wakePrefix.size());
    Log(L"\u6536\u5230\u6d88\u606f\uff1a" + userText);

    auto reply = AskAi(userText);
    if (reply.empty())
    {
        return;
    }
    if (messageType == L"group")
    {
        SendGroupMessage(ReadInt64(event, L"group_id"), reply);
    }
    else if (messageType == L"private")
    {
        SendPrivateMessage(ReadInt64(event, L"user_id"), reply);
    }
}

std::wstring BotEngine::AskAi(std::wstring const& userText)
{
    BotConfig config;
    {
        std::scoped_lock lock(m_mutex);
        config = m_config;
    }
    if (config.aiApiKey.empty())
    {
        Log(L"\u672a\u914d\u7f6e AI API Key");
        return {};
    }

    json::value payload = json::value::object();
    payload[L"model"] = json::value::string(config.aiModel);
    payload[L"temperature"] = json::value::number(0.7);
    payload[L"messages"] = json::value::array();
    json::value systemMessage = json::value::object();
    systemMessage[L"role"] = json::value::string(L"system");
    systemMessage[L"content"] = json::value::string(config.systemPrompt);
    payload[L"messages"][0] = systemMessage;

    json::value userMessage = json::value::object();
    userMessage[L"role"] = json::value::string(L"user");
    userMessage[L"content"] = json::value::string(userText);
    payload[L"messages"][1] = userMessage;

    try
    {
        auto result = PostJson(config.aiBaseUrl + L"/chat/completions", payload, config.aiApiKey);
        return result[L"choices"][0][L"message"][L"content"].as_string();
    }
    catch (std::exception const& ex)
    {
        Log(std::wstring(L"AI \u8bf7\u6c42\u5931\u8d25\uff1a") + winrt::to_hstring(ex.what()).c_str());
        return {};
    }
}

void BotEngine::SendGroupMessage(int64_t groupId, std::wstring const& message)
{
    if (groupId <= 0)
    {
        return;
    }
    json::value payload = json::value::object();
    payload[L"group_id"] = json::value::number(groupId);
    payload[L"message"] = json::value::string(message);

    BotConfig config;
    {
        std::scoped_lock lock(m_mutex);
        config = m_config;
    }
    PostJson(config.onebotApiUrl + L"/send_group_msg", payload, config.onebotAccessToken);
    Log(L"\u5df2\u53d1\u9001\u7fa4\u6d88\u606f");
}

void BotEngine::SendPrivateMessage(int64_t userId, std::wstring const& message)
{
    if (userId <= 0)
    {
        return;
    }
    json::value payload = json::value::object();
    payload[L"user_id"] = json::value::number(userId);
    payload[L"message"] = json::value::string(message);

    BotConfig config;
    {
        std::scoped_lock lock(m_mutex);
        config = m_config;
    }
    PostJson(config.onebotApiUrl + L"/send_private_msg", payload, config.onebotAccessToken);
    Log(L"\u5df2\u53d1\u9001\u79c1\u804a\u6d88\u606f");
}

json::value BotEngine::PostJson(std::wstring const& url, json::value const& payload, std::wstring const& bearerToken)
{
    http_client client(url);
    http_request request(methods::POST);
    request.headers().set_content_type(L"application/json; charset=utf-8");
    if (!bearerToken.empty())
    {
        request.headers().add(L"Authorization", L"Bearer " + bearerToken);
    }
    request.set_body(payload);

    auto response = client.request(request).get();
    auto body = response.extract_string().get();
    if (response.status_code() < 200 || response.status_code() >= 300)
    {
        auto detail = L"HTTP " + std::to_wstring(response.status_code()) + L": " + body;
        throw std::runtime_error(winrt::to_string(winrt::hstring(detail)));
    }
    return body.empty() ? json::value::object() : json::value::parse(body);
}

void BotEngine::Log(std::wstring const& message) const
{
    if (m_logHandler)
    {
        m_logHandler(L"[" + Now() + L"] " + message);
    }
}

std::wstring BotEngine::Now()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    tm localTime{};
    localtime_s(&localTime, &time);

    wchar_t buffer[16]{};
    wcsftime(buffer, std::size(buffer), L"%H:%M:%S", &localTime);
    return buffer;
}

bool BotEngine::HasPrefix(std::wstring const& text, std::wstring const& prefix)
{
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

int64_t BotEngine::ReadInt64(json::value const& json, wchar_t const* key)
{
    if (!json.has_field(key) || !json.at(key).is_number())
    {
        return 0;
    }
    return json.at(key).as_number().to_int64();
}

std::wstring BotEngine::ReadString(json::value const& json, wchar_t const* key)
{
    if (!json.has_field(key))
    {
        return {};
    }
    auto value = json.at(key);
    if (value.is_string())
    {
        return value.as_string();
    }
    return {};
}
