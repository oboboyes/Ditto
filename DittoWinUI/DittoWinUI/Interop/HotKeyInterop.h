#pragma once
#include <windows.h>

namespace winrt::DittoWinUI::Interop
{
    struct HotKeyInterop
    {
        static bool RegisterHotKey(HWND hwnd, int32_t id, UINT modifiers, UINT vk);
        static bool UnregisterHotKey(HWND hwnd, int32_t id);
        static LRESULT CALLBACK HotKeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
}
