#pragma once
#include <winrt/Windows.Foundation.h>

namespace winrt::DittoWinUI::Models
{
    enum class ThemeMode : int32_t
    {
        System = 0,
        Light = 1,
        Dark = 2
    };

    struct AppSettings
    {
        winrt::hstring QuickPasteHotkey{ L"Ctrl+`" };
        bool StartWithWindows{ true };
        bool MinimizeToTray{ true };
        bool ShowNotifications{ true };
        uint32_t MaxClipItems{ 1000 };
        uint32_t MaxStorageMB{ 500 };
        ThemeMode Theme{ ThemeMode::System };
        bool EnableMicaBackdrop{ true };
        bool EnableRoundedCorners{ true };
        bool ConfirmBeforeDelete{ true };
        bool AutoStartClipboardMonitor{ true };
    };
}

namespace winrt::DittoWinUI::Services
{
    struct ISettingsService
    {
        virtual ~ISettingsService() = default;

        virtual Windows::Foundation::IAsyncOperation<Models::AppSettings> LoadSettingsAsync() = 0;
        virtual Windows::Foundation::IAsyncOperation<void> SaveSettingsAsync(Models::AppSettings settings) = 0;
        virtual Windows::Foundation::IAsyncOperation<void> ResetToDefaultsAsync() = 0;

        virtual Models::AppSettings GetSettings() = 0;
        virtual void UpdateSettings(Models::AppSettings settings) = 0;
    };
}
