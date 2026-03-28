#pragma once
#include <windows.h>

namespace winrt::DittoWinUI::Interop
{
    struct ClipboardInterop
    {
        static HWND GetClipboardViewerHwnd();
        static void SetClipboardViewerHwnd(HWND hwnd);
        static void AddClipboardViewer(HWND hwnd);
        static void RemoveClipboardViewer(HWND hwnd);
    };
}
