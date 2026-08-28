#pragma once

#include "App.xaml.g.h"

namespace winrt::QdBot::implementation
{
    struct App : AppT<App>
    {
        App();
        void OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const& args);

    private:
        winrt::Microsoft::UI::Xaml::Window m_window{ nullptr };
    };
}

namespace winrt::QdBot::factory_implementation
{
    struct App : AppT<App, implementation::App>
    {
    };
}
