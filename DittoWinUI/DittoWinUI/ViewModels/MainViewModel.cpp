#include "MainViewModel.h"
#include "QuickPasteViewModel.h"

using namespace winrt::DittoWinUI::ViewModels;
using namespace winrt::DittoWinUI::Services;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Input;

MainViewModel::MainViewModel()
    : m_groups(single_threaded_vector<Models::ClipGroup>())
{
    m_startCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        SetIsRunning(true);
    });

    m_stopCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        SetIsRunning(false);
    });

    m_showSettingsCommand = make<RelayCommand>([](ICommand, IInspectable) {
    });

    m_createGroupCommand = make<RelayCommand>([](ICommand, IInspectable) {
    });

    m_exitCommand = make<RelayCommand>([](ICommand, IInspectable) {
        exit(0);
    });
}

void MainViewModel::SetIsRunning(bool value)
{
    if (m_isRunning != value)
    {
        m_isRunning = value;
        OnPropertyChanged(L"IsRunning");
    }
}

void MainViewModel::SetSettings(Models::AppSettings value)
{
    if (m_settings.QuickPasteHotkey != value.QuickPasteHotkey ||
        m_settings.Theme != value.Theme ||
        m_settings.StartWithWindows != value.StartWithWindows)
    {
        m_settings = value;
        OnPropertyChanged(L"Settings");
    }
}

ICommand MainViewModel::StartCommand()
{
    return *m_startCommand;
}

ICommand MainViewModel::StopCommand()
{
    return *m_stopCommand;
}

ICommand MainViewModel::ShowSettingsCommand()
{
    return *m_showSettingsCommand;
}

ICommand MainViewModel::CreateGroupCommand()
{
    return *m_createGroupCommand;
}

ICommand MainViewModel::ExitCommand()
{
    return *m_exitCommand;
}
