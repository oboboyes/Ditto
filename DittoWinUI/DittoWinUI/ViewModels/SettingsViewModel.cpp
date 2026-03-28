#include "SettingsViewModel.h"

using namespace winrt::DittoWinUI::ViewModels;
using namespace winrt::DittoWinUI::Services;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Input;

SettingsViewModel::SettingsViewModel()
{
    m_settings = Models::AppSettings::GetDefault();

    m_saveCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        SaveSettingsAsync();
    });

    m_resetCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        ResetToDefaults();
    });

    m_browseDatabasePathCommand = make<RelayCommand>([](ICommand, IInspectable) {
    });

    m_testHotkeyCommand = make<RelayCommand>([](ICommand, IInspectable) {
    });
}

void SettingsViewModel::SetSettings(Models::AppSettings value)
{
    m_settings = value;
}

void SettingsViewModel::SetQuickPasteHotkey(winrt::hstring value)
{
    if (m_settings.QuickPasteHotkey != value)
    {
        m_settings.QuickPasteHotkey = value;
        OnPropertyChanged(L"QuickPasteHotkey");
    }
}

void SettingsViewModel::SetStartWithWindows(bool value)
{
    if (m_settings.StartWithWindows != value)
    {
        m_settings.StartWithWindows = value;
        OnPropertyChanged(L"StartWithWindows");
    }
}

void SettingsViewModel::SetMinimizeToTray(bool value)
{
    if (m_settings.MinimizeToTray != value)
    {
        m_settings.MinimizeToTray = value;
        OnPropertyChanged(L"MinimizeToTray");
    }
}

void SettingsViewModel::SetShowNotifications(bool value)
{
    if (m_settings.ShowNotifications != value)
    {
        m_settings.ShowNotifications = value;
        OnPropertyChanged(L"ShowNotifications");
    }
}

void SettingsViewModel::SetMaxClipItems(uint32_t value)
{
    if (m_settings.MaxClipItems != value)
    {
        m_settings.MaxClipItems = value;
        OnPropertyChanged(L"MaxClipItems");
    }
}

void SettingsViewModel::SetMaxStorageMB(uint32_t value)
{
    if (m_settings.MaxStorageMB != value)
    {
        m_settings.MaxStorageMB = value;
        OnPropertyChanged(L"MaxStorageMB");
    }
}

void SettingsViewModel::SetTheme(Models::ThemeMode value)
{
    if (m_settings.Theme != value)
    {
        m_settings.Theme = value;
        OnPropertyChanged(L"Theme");
    }
}

void SettingsViewModel::SetEnableMicaBackdrop(bool value)
{
    if (m_settings.EnableMicaBackdrop != value)
    {
        m_settings.EnableMicaBackdrop = value;
        OnPropertyChanged(L"EnableMicaBackdrop");
    }
}

void SettingsViewModel::SetEnableRoundedCorners(bool value)
{
    if (m_settings.EnableRoundedCorners != value)
    {
        m_settings.EnableRoundedCorners = value;
        OnPropertyChanged(L"EnableRoundedCorners");
    }
}

void SettingsViewModel::SetConfirmBeforeDelete(bool value)
{
    if (m_settings.ConfirmBeforeDelete != value)
    {
        m_settings.ConfirmBeforeDelete = value;
        OnPropertyChanged(L"ConfirmBeforeDelete");
    }
}

void SettingsViewModel::SetAutoStartClipboardMonitor(bool value)
{
    if (m_settings.AutoStartClipboardMonitor != value)
    {
        m_settings.AutoStartClipboardMonitor = value;
        OnPropertyChanged(L"AutoStartClipboardMonitor");
    }
}

IAsyncOperation<void> SettingsViewModel::LoadSettingsAsync()
{
    co_await resume_background();

    auto& settingsService = ServiceLocator::Instance().Settings();
    m_settings = co_await settingsService.LoadSettingsAsync();

    OnPropertyChanged(L"Settings");
    OnPropertyChanged(L"QuickPasteHotkey");
    OnPropertyChanged(L"StartWithWindows");
    OnPropertyChanged(L"MinimizeToTray");
    OnPropertyChanged(L"ShowNotifications");
    OnPropertyChanged(L"MaxClipItems");
    OnPropertyChanged(L"MaxStorageMB");
    OnPropertyChanged(L"Theme");
    OnPropertyChanged(L"EnableMicaBackdrop");
    OnPropertyChanged(L"EnableRoundedCorners");
    OnPropertyChanged(L"ConfirmBeforeDelete");
    OnPropertyChanged(L"AutoStartClipboardMonitor");

    co_return;
}

IAsyncOperation<void> SettingsViewModel::SaveSettingsAsync()
{
    co_await resume_background();

    auto& settingsService = ServiceLocator::Instance().Settings();
    co_await settingsService.SaveSettingsAsync(m_settings);

    co_return;
}

void SettingsViewModel::ResetToDefaults()
{
    m_settings = Models::AppSettings::GetDefault();
    OnPropertyChanged(L"Settings");
}

ICommand SettingsViewModel::SaveCommand()
{
    return *m_saveCommand;
}

ICommand SettingsViewModel::ResetCommand()
{
    return *m_resetCommand;
}

ICommand SettingsViewModel::BrowseDatabasePathCommand()
{
    return *m_browseDatabasePathCommand;
}

ICommand SettingsViewModel::TestHotkeyCommand()
{
    return *m_testHotkeyCommand;
}
