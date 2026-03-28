#include "HotKeyInterop.h"
#include <functional>

namespace
{
    std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> g_wndProcCallback;
}

namespace winrt::DittoWinUI::Interop
{
    bool HotKeyInterop::RegisterHotKey(HWND hwnd, int32_t id, UINT modifiers, UINT vk)
    {
        return ::RegisterHotKey(hwnd, id, modifiers, vk) != 0;
    }

    bool HotKeyInterop::UnregisterHotKey(HWND hwnd, int32_t id)
    {
        return ::UnregisterHotKey(hwnd, id) != 0;
    }

    LRESULT CALLBACK HotKeyInterop::HotKeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_HOTKEY)
        {
            int32_t hotKeyId = static_cast<int32_t>(wParam);
        }

        if (g_wndProcCallback)
        {
            return g_wndProcCallback(hwnd, msg, wParam, lParam);
        }

        return ::DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void SetWndProcCallback(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> callback)
    {
        g_wndProcCallback = callback;
    }
}
