#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt::QdBot::implementation
{
    App::App()
    {
        InitializeComponent();
    }

    void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
        m_window = winrt::make<MainWindow>();
        m_window.Activate();
    }
}
