#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>

namespace winrt::DittoWinUI::Services
{
    struct IThemeService
    {
        virtual ~IThemeService() = default;

        virtual Windows::Foundation::IAsyncOperation<void> SetThemeAsync(Models::ThemeMode mode) = 0;
        virtual Models::ThemeMode GetCurrentTheme() const = 0;
        virtual bool IsDarkMode() const = 0;
    };

    struct ThemeService : IThemeService
    {
    public:
        ThemeService();

        Windows::Foundation::IAsyncOperation<void> SetThemeAsync(Models::ThemeMode mode) override;
        Models::ThemeMode GetCurrentTheme() const override { return m_currentTheme; }
        bool IsDarkMode() const override;

        void ApplyThemeToWindow(HWND hwnd);

    private:
        Models::ThemeMode m_currentTheme{ Models::ThemeMode::System };
    };
}
