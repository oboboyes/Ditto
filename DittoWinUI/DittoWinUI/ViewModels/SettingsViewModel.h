#pragma once
#include "ViewModelBase.h"
#include "Models/ClipGroup.h"

namespace winrt::DittoWinUI::ViewModels
{
    struct SettingsViewModel : ViewModelBase
    {
    public:
        SettingsViewModel();

        Models::AppSettings Settings() const { return m_settings; }
        void SetSettings(Models::AppSettings value);

        winrt::hstring QuickPasteHotkey() const { return m_settings.QuickPasteHotkey; }
        void SetQuickPasteHotkey(winrt::hstring value);

        bool StartWithWindows() const { return m_settings.StartWithWindows; }
        void SetStartWithWindows(bool value);

        bool MinimizeToTray() const { return m_settings.MinimizeToTray; }
        void SetMinimizeToTray(bool value);

        bool ShowNotifications() const { return m_settings.ShowNotifications; }
        void SetShowNotifications(bool value);

        uint32_t MaxClipItems() const { return m_settings.MaxClipItems; }
        void SetMaxClipItems(uint32_t value);

        uint32_t MaxStorageMB() const { return m_settings.MaxStorageMB; }
        void SetMaxStorageMB(uint32_t value);

        Models::ThemeMode Theme() const { return m_settings.Theme; }
        void SetTheme(Models::ThemeMode value);

        bool EnableMicaBackdrop() const { return m_settings.EnableMicaBackdrop; }
        void SetEnableMicaBackdrop(bool value);

        bool EnableRoundedCorners() const { return m_settings.EnableRoundedCorners; }
        void SetEnableRoundedCorners(bool value);

        bool ConfirmBeforeDelete() const { return m_settings.ConfirmBeforeDelete; }
        void SetConfirmBeforeDelete(bool value);

        bool AutoStartClipboardMonitor() const { return m_settings.AutoStartClipboardMonitor; }
        void SetAutoStartClipboardMonitor(bool value);

        winrt::Windows::UI::Xaml::Input::ICommand SaveCommand();
        winrt::Windows::UI::Xaml::Input::ICommand ResetCommand();
        winrt::Windows::UI::Xaml::Input::ICommand BrowseDatabasePathCommand();
        winrt::Windows::UI::Xaml::Input::ICommand TestHotkeyCommand();

        Windows::Foundation::IAsyncOperation<void> LoadSettingsAsync();
        Windows::Foundation::IAsyncOperation<void> SaveSettingsAsync();
        void ResetToDefaults();

    private:
        Models::AppSettings m_settings;
        winrt::com_ptr<RelayCommand> m_saveCommand;
        winrt::com_ptr<RelayCommand> m_resetCommand;
        winrt::com_ptr<RelayCommand> m_browseDatabasePathCommand;
        winrt::com_ptr<RelayCommand> m_testHotkeyCommand;
    };
}
