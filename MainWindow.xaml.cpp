#include "pch.h"
#include "MainWindow.xaml.h"
#include "ConfigManager.h"

#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::QdBot::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        m_config = ConfigManager::Load();
        m_engine = std::make_shared<BotEngine>([weakThis = get_weak()](std::wstring const& message)
        {
            if (auto self = weakThis.get())
            {
                self->DispatcherQueue().TryEnqueue([self, message]
                {
                    self->AppendLog(message);
                });
            }
        });
        m_engine->Configure(m_config);
        LoadConfigToUi();
    }

    void MainWindow::SaveButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SaveConfigFromUi();
    }

    void MainWindow::StartButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SaveConfigFromUi();
        try
        {
            m_engine->Start();
            SetStatus(L"运行中");
        }
        catch (std::exception const& ex)
        {
            AppendLog(std::wstring(L"启动失败：") + to_hstring(ex.what()).c_str());
        }
    }

    void MainWindow::StopButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_engine->Stop();
        SetStatus(L"未启动");
    }

    void MainWindow::TestAiButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SaveConfigFromUi();
        m_engine->TestAi();
    }

    void MainWindow::LoadConfigToUi()
    {
        HostBox().Text(m_config.host);
        PortBox().Value(m_config.port);
        OneBotUrlBox().Text(m_config.onebotApiUrl);
        OneBotTokenBox().Password(m_config.onebotAccessToken);
        AiBaseUrlBox().Text(m_config.aiBaseUrl);
        AiApiKeyBox().Password(m_config.aiApiKey);
        AiModelBox().Text(m_config.aiModel);
        WakePrefixBox().Text(m_config.wakePrefix);
        OwnerJoinTextBox().Text(m_config.ownerJoinText);
        SystemPromptBox().Text(m_config.systemPrompt);
        ReplyGroupCheck().IsChecked(m_config.replyGroup);
        ReplyPrivateCheck().IsChecked(m_config.replyPrivate);
        ModeBox().SelectedIndex(m_config.mode == L"词库" ? 1 : 0);
    }

    BotConfig MainWindow::ReadConfigFromUi()
    {
        BotConfig config;
        config.host = HostBox().Text().c_str();
        config.port = static_cast<int>(PortBox().Value());
        config.onebotApiUrl = OneBotUrlBox().Text().c_str();
        config.onebotAccessToken = OneBotTokenBox().Password().c_str();
        config.aiBaseUrl = AiBaseUrlBox().Text().c_str();
        config.aiApiKey = AiApiKeyBox().Password().c_str();
        config.aiModel = AiModelBox().Text().c_str();
        config.wakePrefix = WakePrefixBox().Text().c_str();
        config.ownerJoinText = OwnerJoinTextBox().Text().c_str();
        config.systemPrompt = SystemPromptBox().Text().c_str();
        config.replyGroup = ReplyGroupCheck().IsChecked().GetBoolean();
        config.replyPrivate = ReplyPrivateCheck().IsChecked().GetBoolean();
        config.mode = ModeBox().SelectedIndex() == 1 ? L"词库" : L"AI";
        return config;
    }

    void MainWindow::SaveConfigFromUi()
    {
        m_config = ReadConfigFromUi();
        ConfigManager::Save(m_config);
        m_engine->Configure(m_config);
        AppendLog(L"配置已保存");
    }

    void MainWindow::AppendLog(std::wstring const& message)
    {
        auto current = std::wstring(LogBox().Text().c_str());
        if (!current.empty())
        {
            current += L"\r\n";
        }
        LogBox().Text(current + message);
    }

    void MainWindow::SetStatus(std::wstring const& status)
    {
        StatusText().Text(status);
    }
}
