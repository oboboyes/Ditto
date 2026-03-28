#include "ClipboardInterop.h"

namespace
{
    HWND g_clipboardViewerHwnd = nullptr;
}

namespace winrt::DittoWinUI::Interop
{
    HWND ClipboardInterop::GetClipboardViewerHwnd()
    {
        if (!g_clipboardViewerHwnd)
        {
            g_clipboardViewerHwnd = ::GetForegroundWindow();
        }
        return g_clipboardViewerHwnd;
    }

    void ClipboardInterop::SetClipboardViewerHwnd(HWND hwnd)
    {
        g_clipboardViewerHwnd = hwnd;
    }

    void ClipboardInterop::AddClipboardViewer(HWND hwnd)
    {
        ::SetClipboardViewer(hwnd);
    }

    void ClipboardInterop::RemoveClipboardViewer(HWND hwnd)
    {
        ::ChangeClipboardChain(hwnd, ::GetNextClipboardViewer(hwnd));
    }
}
