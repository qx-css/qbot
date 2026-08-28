#pragma once

#include "BotConfig.h"

class ConfigManager
{
public:
    static BotConfig Load();
    static void Save(BotConfig const& config);

private:
    static std::filesystem::path ConfigPath();
};
