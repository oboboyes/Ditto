#include "ThemeInterop.h"
#include <dwmapi.h>
#include <winreg.h>

#pragma comment(lib, "dwmapi.lib")

namespace winrt::DittoWinUI::Interop
{
    bool ThemeInterop::SetWindowBackdrop(HWND hwnd, BackdropType type)
    {
        if (!hwnd) return false;

        DWMWINDOWATTRIBUTE attribute = DWMWA_USE_IMMERSIVE_DARK_MODE;
        switch (type)
        {
        case BackdropType::Mica:
            attribute = DWMWA_SYSTEMBACKDROP_TYPE;
            break;
        case BackdropType::Acrylic:
            attribute = DWMWA_SYSTEMBACKDROP_TYPE;
            break;
        default:
            attribute = DWMWA_SYSTEMBACKDROP_TYPE;
            break;
        }

        int value = static_cast<int>(type);
        return SUCCEEDED(::DwmSetWindowAttribute(hwnd, attribute, &value, sizeof(value)));
    }

    bool ThemeInterop::SetWindowRoundedCorners(HWND hwnd, bool enable)
    {
        if (!hwnd) return false;

        DWMWINDOWATTRIBUTE attribute = DWMWA_WINDOW_CORNER_PREFERENCE;
        int value = enable ? 2 : 0; 
        return SUCCEEDED(::DwmSetWindowAttribute(hwnd, attribute, &value, sizeof(value)));
    }

    bool ThemeInterop::SetSystemTheme(bool darkMode)
    {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
        {
            DWORD value = darkMode ? 1 : 0;
            RegSetValueExW(hKey, L"AppsUseLightTheme", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
            RegCloseKey(hKey);
            return true;
        }
        return false;
    }

    bool ThemeInterop::IsSystemDarkMode()
    {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            DWORD value = 1;
            DWORD size = sizeof(value);
            RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, (LPBYTE)&value, &size);
            RegCloseKey(hKey);
            return value == 0;
        }
        return false;
    }
}
