#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Windows.ApplicationModel.Activation.h>

using namespace winrt::DittoWinUI;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::ApplicationModel::Activation;

namespace
{
    Windows::UI::Core::CoreWindow GetCoreWindow()
    {
        return Windows::UI::Core::CoreWindow::GetForCurrentThread();
    }
}

App::App()
{
    Initialize();
}

void App::OnLaunched(LaunchActivatedEventArgs const& args)
{
    auto window = Window::Current();
    if (window)
    {
        window.Activate();
    }
}

void App::OnActivated(IActivatedEventArgs const& args)
{
    if (args.Kind() == ActivationKind::Launch ||
        args.Kind() == ActivationKind::Protocol)
    {
        SetWindow();
        auto window = Window::Current();
        if (window)
        {
            window.Activate();
        }
    }
}

void App::Initialize()
{
}

void App::SetWindow()
{
    auto window = Window::Current();
    if (!window)
    {
        window = Window::Create();
        window.Content(MainWindow());
        window.Title(L"Ditto WinUI 3");
    }
}
