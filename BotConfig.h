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
    std::wstring systemPrompt = L"\u4f60\u662f\u4e00\u4e2a\u53cb\u597d\u7684 QQ \u7fa4\u52a9\u624b\uff0c\u56de\u7b54\u8981\u7b80\u6d01\u3001\u81ea\u7136\u3001\u6709\u5e2e\u52a9\u3002";
    std::wstring mode = L"AI";
    bool replyPrivate = true;
    bool replyGroup = true;
    std::wstring wakePrefix;
    std::wstring ownerJoinText = L"\u4e3b\u4eba\uff0c\u6211\u6765\u54af\uff01\uff01\uff01";
};
