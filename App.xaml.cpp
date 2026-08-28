#include "pch.h"
#include "App.xaml.h"
#include "ConfigManager.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::QdBot::implementation
{
    App::App()
    {
    }

    void App::OnLaunched(LaunchActivatedEventArgs const&)
    {
        m_config = ConfigManager::Load();
        m_engine = std::make_shared<BotEngine>([this](std::wstring const& message)
        {
            if (m_window)
            {
                m_window.DispatcherQueue().TryEnqueue([this, message]
                {
                    AppendLog(message);
                });
            }
        });
        m_engine->Configure(m_config);
        BuildUi();
        LoadConfigToUi();
        m_window.Activate();
    }

    void App::BuildUi()
    {
        m_window = Window();
        m_window.Title(L"QdBot");

        StackPanel root;
        root.Padding(ThicknessHelper::FromUniformLength(22));
        root.Spacing(12);
        root.Orientation(Orientation::Vertical);

        TextBlock title;
        title.Text(L"QdBot");
        title.FontSize(28);
        title.FontWeight(Windows::UI::Text::FontWeights::SemiBold());
        root.Children().Append(title);

        m_statusText = TextBlock();
        m_statusText.Text(L"\u672a\u542f\u52a8");
        root.Children().Append(m_statusText);

        ScrollViewer scrollViewer;
        StackPanel configPanel;
        configPanel.Spacing(10);
        AddLabeledControls(configPanel);
        scrollViewer.Content(configPanel);
        root.Children().Append(scrollViewer);

        StackPanel buttonPanel;
        buttonPanel.Orientation(Orientation::Horizontal);
        buttonPanel.Spacing(10);

        Button saveButton;
        saveButton.Content(box_value(L"\u4fdd\u5b58\u914d\u7f6e"));
        saveButton.Click({ this, &App::SaveButton_Click });
        buttonPanel.Children().Append(saveButton);

        Button startButton;
        startButton.Content(box_value(L"\u542f\u52a8"));
        startButton.Click({ this, &App::StartButton_Click });
        buttonPanel.Children().Append(startButton);

        Button stopButton;
        stopButton.Content(box_value(L"\u505c\u6b62"));
        stopButton.Click({ this, &App::StopButton_Click });
        buttonPanel.Children().Append(stopButton);

        Button testButton;
        testButton.Content(box_value(L"\u6d4b\u8bd5 AI"));
        testButton.Click({ this, &App::TestAiButton_Click });
        buttonPanel.Children().Append(testButton);
        root.Children().Append(buttonPanel);

        m_logBox = TextBox();
        m_logBox.Header(box_value(L"\u8fd0\u884c\u65e5\u5fd7"));
        m_logBox.AcceptsReturn(true);
        m_logBox.IsReadOnly(true);
        m_logBox.TextWrapping(TextWrapping::Wrap);
        m_logBox.MinHeight(220);
        root.Children().Append(m_logBox);

        m_window.Content(root);
    }

    void App::AddLabeledControls(StackPanel const& panel)
    {
        m_hostBox = TextBox();
        m_hostBox.Header(box_value(L"\u76d1\u542c\u5730\u5740"));
        panel.Children().Append(m_hostBox);

        m_portBox = NumberBox();
        m_portBox.Header(box_value(L"\u76d1\u542c\u7aef\u53e3"));
        m_portBox.Minimum(1);
        m_portBox.Maximum(65535);
        panel.Children().Append(m_portBox);

        m_oneBotUrlBox = TextBox();
        m_oneBotUrlBox.Header(box_value(L"OneBot API \u5730\u5740"));
        panel.Children().Append(m_oneBotUrlBox);

        m_oneBotTokenBox = PasswordBox();
        m_oneBotTokenBox.Header(box_value(L"OneBot Token"));
        panel.Children().Append(m_oneBotTokenBox);

        m_modeBox = ComboBox();
        m_modeBox.Header(box_value(L"\u6a21\u5f0f"));
        ComboBoxItem aiItem;
        aiItem.Content(box_value(L"AI"));
        ComboBoxItem lexiconItem;
        lexiconItem.Content(box_value(L"\u8bcd\u5e93"));
        m_modeBox.Items().Append(aiItem);
        m_modeBox.Items().Append(lexiconItem);
        panel.Children().Append(m_modeBox);

        m_aiBaseUrlBox = TextBox();
        m_aiBaseUrlBox.Header(box_value(L"AI Base URL"));
        panel.Children().Append(m_aiBaseUrlBox);

        m_aiApiKeyBox = PasswordBox();
        m_aiApiKeyBox.Header(box_value(L"AI API Key"));
        panel.Children().Append(m_aiApiKeyBox);

        m_aiModelBox = TextBox();
        m_aiModelBox.Header(box_value(L"AI \u6a21\u578b"));
        panel.Children().Append(m_aiModelBox);

        m_wakePrefixBox = TextBox();
        m_wakePrefixBox.Header(box_value(L"\u89e6\u53d1\u524d\u7f00\uff0c\u53ef\u7559\u7a7a"));
        panel.Children().Append(m_wakePrefixBox);

        m_ownerJoinTextBox = TextBox();
        m_ownerJoinTextBox.Header(box_value(L"\u8fdb\u7fa4\u63d0\u793a"));
        panel.Children().Append(m_ownerJoinTextBox);

        m_systemPromptBox = TextBox();
        m_systemPromptBox.Header(box_value(L"\u7cfb\u7edf\u63d0\u793a\u8bcd"));
        m_systemPromptBox.AcceptsReturn(true);
        m_systemPromptBox.TextWrapping(TextWrapping::Wrap);
        m_systemPromptBox.MinHeight(100);
        panel.Children().Append(m_systemPromptBox);

        StackPanel checkPanel;
        checkPanel.Orientation(Orientation::Horizontal);
        checkPanel.Spacing(16);
        m_replyGroupCheck = CheckBox();
        m_replyGroupCheck.Content(box_value(L"\u56de\u590d\u7fa4\u804a"));
        checkPanel.Children().Append(m_replyGroupCheck);
        m_replyPrivateCheck = CheckBox();
        m_replyPrivateCheck.Content(box_value(L"\u56de\u590d\u79c1\u804a"));
        checkPanel.Children().Append(m_replyPrivateCheck);
        panel.Children().Append(checkPanel);
    }

    void App::SaveButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SaveConfigFromUi();
    }

    void App::StartButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SaveConfigFromUi();
        try
        {
            m_engine->Start();
            SetStatus(L"\u8fd0\u884c\u4e2d");
        }
        catch (std::exception const& ex)
        {
            AppendLog(std::wstring(L"\u542f\u52a8\u5931\u8d25\uff1a") + to_hstring(ex.what()).c_str());
        }
    }

    void App::StopButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_engine->Stop();
        SetStatus(L"\u672a\u542f\u52a8");
    }

    void App::TestAiButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SaveConfigFromUi();
        m_engine->TestAi();
    }

    void App::LoadConfigToUi()
    {
        m_hostBox.Text(m_config.host);
        m_portBox.Value(m_config.port);
        m_oneBotUrlBox.Text(m_config.onebotApiUrl);
        m_oneBotTokenBox.Password(m_config.onebotAccessToken);
        m_aiBaseUrlBox.Text(m_config.aiBaseUrl);
        m_aiApiKeyBox.Password(m_config.aiApiKey);
        m_aiModelBox.Text(m_config.aiModel);
        m_wakePrefixBox.Text(m_config.wakePrefix);
        m_ownerJoinTextBox.Text(m_config.ownerJoinText);
        m_systemPromptBox.Text(m_config.systemPrompt);
        m_replyGroupCheck.IsChecked(m_config.replyGroup);
        m_replyPrivateCheck.IsChecked(m_config.replyPrivate);
        m_modeBox.SelectedIndex(m_config.mode == L"\u8bcd\u5e93" ? 1 : 0);
    }

    BotConfig App::ReadConfigFromUi()
    {
        BotConfig config;
        config.host = m_hostBox.Text().c_str();
        config.port = static_cast<int>(m_portBox.Value());
        config.onebotApiUrl = m_oneBotUrlBox.Text().c_str();
        config.onebotAccessToken = m_oneBotTokenBox.Password().c_str();
        config.aiBaseUrl = m_aiBaseUrlBox.Text().c_str();
        config.aiApiKey = m_aiApiKeyBox.Password().c_str();
        config.aiModel = m_aiModelBox.Text().c_str();
        config.wakePrefix = m_wakePrefixBox.Text().c_str();
        config.ownerJoinText = m_ownerJoinTextBox.Text().c_str();
        config.systemPrompt = m_systemPromptBox.Text().c_str();
        auto replyGroup = m_replyGroupCheck.IsChecked();
        auto replyPrivate = m_replyPrivateCheck.IsChecked();
        config.replyGroup = replyGroup && replyGroup.Value();
        config.replyPrivate = replyPrivate && replyPrivate.Value();
        config.mode = m_modeBox.SelectedIndex() == 1 ? L"\u8bcd\u5e93" : L"AI";
        return config;
    }

    void App::SaveConfigFromUi()
    {
        m_config = ReadConfigFromUi();
        ConfigManager::Save(m_config);
        m_engine->Configure(m_config);
        AppendLog(L"\u914d\u7f6e\u5df2\u4fdd\u5b58");
    }

    void App::AppendLog(std::wstring const& message)
    {
        auto current = std::wstring(m_logBox.Text().c_str());
        if (!current.empty())
        {
            current += L"\r\n";
        }
        m_logBox.Text(hstring{ current + message });
    }

    void App::SetStatus(std::wstring const& status)
    {
        m_statusText.Text(status);
    }
}
