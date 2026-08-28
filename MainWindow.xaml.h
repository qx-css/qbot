#pragma once

#include "BotConfig.h"
#include "BotEngine.h"
#include "MainWindow.g.h"

namespace winrt::QdBot::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void SaveButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void StartButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void StopButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void TestAiButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        void LoadConfigToUi();
        BotConfig ReadConfigFromUi();
        void SaveConfigFromUi();
        void AppendLog(std::wstring const& message);
        void SetStatus(std::wstring const& status);

        BotConfig m_config;
        std::shared_ptr<BotEngine> m_engine;
    };
}

namespace winrt::QdBot::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
