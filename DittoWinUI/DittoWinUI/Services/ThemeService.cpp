#include "IThemeService.h"
#include "ThemeInterop.h"

namespace winrt::DittoWinUI::Services
{
    ThemeService::ThemeService()
    {
        m_currentTheme = Models::ThemeMode::System;
    }

    Windows::Foundation::IAsyncOperation<void> ThemeService::SetThemeAsync(Models::ThemeMode mode)
    {
        co_await winrt::resume_background();

        m_currentTheme = mode;

        bool useDark = false;
        if (mode == Models::ThemeMode::System)
        {
            useDark = Interop::ThemeInterop::IsSystemDarkMode();
        }
        else if (mode == Models::ThemeMode::Dark)
        {
            useDark = true;
        }

        if (useDark)
        {
            Interop::ThemeInterop::SetSystemTheme(true);
        }
        else
        {
            Interop::ThemeInterop::SetSystemTheme(false);
        }

        co_return;
    }

    bool ThemeService::IsDarkMode() const
    {
        if (m_currentTheme == Models::ThemeMode::System)
        {
            return Interop::ThemeInterop::IsSystemDarkMode();
        }
        return m_currentTheme == Models::ThemeMode::Dark;
    }

    void ThemeService::ApplyThemeToWindow(HWND hwnd)
    {
        if (m_currentTheme == Models::ThemeMode::Dark)
        {
            Interop::ThemeInterop::SetWindowBackdrop(hwnd, Interop::ThemeInterop::BackdropType::Mica);
        }
        else
        {
            Interop::ThemeInterop::SetWindowBackdrop(hwnd, Interop::ThemeInterop::BackdropType::Mica);
        }
        Interop::ThemeInterop::SetWindowRoundedCorners(hwnd, true);
    }
}
