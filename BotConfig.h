#pragma once

#include <string>

struct BotConfig
{
    std::wstring host = L"127.0.0.1";
    int port = 8765;
    std::wstring onebotApiUrl = L"http://127.0.0.1:3000";
    std::wstring onebotAccessToken;
    std::wstring aiBaseUrl = L"https://api.openai.com/v1";
    std::wstring aiApiKey;
    std::wstring aiModel = L"gpt-4o-mini";
    std::wstring systemPrompt = L"你是一个友好的 QQ 群助手，回答要简洁、自然、有帮助。";
    std::wstring mode = L"AI";
    bool replyPrivate = true;
    bool replyGroup = true;
    std::wstring wakePrefix;
    std::wstring ownerJoinText = L"主人，我来咯！！！";
};
