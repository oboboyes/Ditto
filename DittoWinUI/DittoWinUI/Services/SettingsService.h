#pragma once
#include "ISettingsService.h"

namespace winrt::DittoWinUI::Services
{
    struct SettingsService : ISettingsService
    {
    public:
        SettingsService();

        Windows::Foundation::IAsyncOperation<Models::AppSettings> LoadSettingsAsync() override;
        Windows::Foundation::IAsyncOperation<void> SaveSettingsAsync(Models::AppSettings settings) override;
        Windows::Foundation::IAsyncOperation<void> ResetToDefaultsAsync() override;

        Models::AppSettings GetSettings() override;
        void UpdateSettings(Models::AppSettings settings) override;

    private:
        static std::wstring SerializeSettingsToJson(const Models::AppSettings& settings);
        static Models::AppSettings ParseSettingsJson(const winrt::hstring& json);

        Models::AppSettings m_settings;
    };
}

namespace winrt::DittoWinUI::Models
{
    inline AppSettings AppSettings::GetDefault()
    {
        return DittoWinUI::Services::SettingsService::GetDefaultImpl();
    }
}
