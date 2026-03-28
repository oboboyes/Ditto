#include "SettingsService.h"
#include <winrt/Windows.Storage.h>

using namespace winrt::DittoWinUI::Services;
using namespace winrt::DittoWinUI::Models;

SettingsService::SettingsService()
{
    m_settings = AppSettings::GetDefault();
}

Windows::Foundation::IAsyncOperation<AppSettings> SettingsService::LoadSettingsAsync()
{
    co_await winrt::resume_background();

    try
    {
        auto folder = Windows::Storage::ApplicationData::Current().LocalFolder();
        auto file = co_await folder.GetFileAsync(L"settings.json");
        auto content = co_await Windows::Storage::FileIO::ReadTextAsync(file);

        if (!content.empty())
        {
            m_settings = ParseSettingsJson(content);
        }
    }
    catch (...)
    {
        m_settings = AppSettings::GetDefault();
    }

    co_return m_settings;
}

Windows::Foundation::IAsyncOperation<void> SettingsService::SaveSettingsAsync(AppSettings settings)
{
    co_await winrt::resume_background();

    try
    {
        auto folder = Windows::Storage::ApplicationData::Current().LocalFolder();
        auto file = co_await folder.CreateFileAsync(L"settings.json", Windows::Storage::CreationCollisionOption::ReplaceExisting);

        std::wstring json = SerializeSettingsToJson(settings);
        co_await Windows::Storage::FileIO::WriteTextAsync(file, json);

        m_settings = settings;
    }
    catch (...)
    {
    }

    co_return;
}

Windows::Foundation::IAsyncOperation<void> SettingsService::ResetToDefaultsAsync()
{
    co_await winrt::resume_background();
    m_settings = AppSettings::GetDefault();
    co_return SaveSettingsAsync(m_settings);
}

AppSettings SettingsService::GetSettings()
{
    return m_settings;
}

void SettingsService::UpdateSettings(AppSettings settings)
{
    m_settings = settings;
}

AppSettings AppSettings::GetDefault()
{
    AppSettings defaults;
    defaults.QuickPasteHotkey = L"Ctrl+`";
    defaults.StartWithWindows = true;
    defaults.MinimizeToTray = true;
    defaults.ShowNotifications = true;
    defaults.MaxClipItems = 1000;
    defaults.MaxStorageMB = 500;
    defaults.Theme = ThemeMode::System;
    defaults.EnableMicaBackdrop = true;
    defaults.EnableRoundedCorners = true;
    defaults.ConfirmBeforeDelete = true;
    defaults.AutoStartClipboardMonitor = true;
    return defaults;
}

std::wstring SettingsService::SerializeSettingsToJson(const AppSettings& settings)
{
    std::wstring json = L"{";
    json += L"\"QuickPasteHotkey\":\"" + settings.QuickPasteHotkey + L"\",";
    json += L"\"StartWithWindows\":" + std::to_wstring(settings.StartWithWindows ? 1 : 0) + L",";
    json += L"\"MinimizeToTray\":" + std::to_wstring(settings.MinimizeToTray ? 1 : 0) + L",";
    json += L"\"ShowNotifications\":" + std::to_wstring(settings.ShowNotifications ? 1 : 0) + L",";
    json += L"\"MaxClipItems\":" + std::to_wstring(settings.MaxClipItems) + L",";
    json += L"\"MaxStorageMB\":" + std::to_wstring(settings.MaxStorageMB) + L",";
    json += L"\"Theme\":" + std::to_wstring(static_cast<int>(settings.Theme)) + L",";
    json += L"\"EnableMicaBackdrop\":" + std::to_wstring(settings.EnableMicaBackdrop ? 1 : 0) + L",";
    json += L"\"EnableRoundedCorners\":" + std::to_wstring(settings.EnableRoundedCorners ? 1 : 0) + L",";
    json += L"\"ConfirmBeforeDelete\":" + std::to_wstring(settings.ConfirmBeforeDelete ? 1 : 0) + L",";
    json += L"\"AutoStartClipboardMonitor\":" + std::to_wstring(settings.AutoStartClipboardMonitor ? 1 : 0);
    json += L"}";
    return json;
}

AppSettings SettingsService::ParseSettingsJson(const winrt::hstring& json)
{
    AppSettings settings = AppSettings::GetDefault();
    return settings;
}
