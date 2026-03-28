#pragma once
#include "ViewModelBase.h"
#include <winrt/Windows.Foundation.Collections.h>
#include "Services/ISettingsService.h"
#include "Models/ClipGroup.h"

namespace winrt::DittoWinUI::ViewModels
{
    struct MainViewModel : ViewModelBase
    {
    public:
        MainViewModel();

        bool IsRunning() const { return m_isRunning; }
        void SetIsRunning(bool value);

        winrt::Windows::Foundation::Collections::IVector<Models::ClipGroup> Groups();
        Models::AppSettings Settings() const { return m_settings; }
        void SetSettings(Models::AppSettings value);

        winrt::Windows::UI::Xaml::Input::ICommand StartCommand();
        winrt::Windows::UI::Xaml::Input::ICommand StopCommand();
        winrt::Windows::UI::Xaml::Input::ICommand ShowSettingsCommand();
        winrt::Windows::UI::Xaml::Input::ICommand CreateGroupCommand();
        winrt::Windows::UI::Xaml::Input::ICommand ExitCommand();

    private:
        bool m_isRunning{ false };
        Models::AppSettings m_settings;
        winrt::Windows::Foundation::Collections::IVector<Models::ClipGroup> m_groups;
        winrt::com_ptr<RelayCommand> m_startCommand;
        winrt::com_ptr<RelayCommand> m_stopCommand;
        winrt::com_ptr<RelayCommand> m_showSettingsCommand;
        winrt::com_ptr<RelayCommand> m_createGroupCommand;
        winrt::com_ptr<RelayCommand> m_exitCommand;
    };
}
