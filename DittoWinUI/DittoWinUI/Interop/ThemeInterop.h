#pragma once
#include <windows.h>

namespace winrt::DittoWinUI::Interop
{
    struct ThemeInterop
    {
        enum class BackdropType : int32_t
        {
            None = 0,
            Mica = 2,
            Acrylic = 3
        };

        static bool SetWindowBackdrop(HWND hwnd, BackdropType type);
        static bool SetWindowRoundedCorners(HWND hwnd, bool enable);
        static bool SetSystemTheme(bool darkMode);
        static bool IsSystemDarkMode();
    };
}
