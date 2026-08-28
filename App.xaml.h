#pragma once

#include "BotConfig.h"
#include "BotEngine.h"

namespace winrt::QdBot::implementation
{
    struct App : winrt::Microsoft::UI::Xaml::ApplicationT<App>
    {
        App();
        void OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const& args);

    private:
        void BuildUi();
        void AddLabeledControls(winrt::Microsoft::UI::Xaml::Controls::StackPanel const& panel);
        void SaveButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void StartButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void StopButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void TestAiButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void LoadConfigToUi();
        BotConfig ReadConfigFromUi();
        void SaveConfigFromUi();
        void AppendLog(std::wstring const& message);
        void SetStatus(std::wstring const& status);

        winrt::Microsoft::UI::Xaml::Window m_window{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBlock m_statusText{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_hostBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::NumberBox m_portBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_oneBotUrlBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::PasswordBox m_oneBotTokenBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::ComboBox m_modeBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_aiBaseUrlBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::PasswordBox m_aiApiKeyBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_aiModelBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_wakePrefixBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_ownerJoinTextBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_systemPromptBox{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::CheckBox m_replyGroupCheck{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::CheckBox m_replyPrivateCheck{ nullptr };
        winrt::Microsoft::UI::Xaml::Controls::TextBox m_logBox{ nullptr };

        BotConfig m_config;
        std::shared_ptr<BotEngine> m_engine;
    };
}
