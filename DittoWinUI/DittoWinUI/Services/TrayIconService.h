#pragma once
#include <winrt/Windows.Foundation.h>
#include <shellapi.h>

namespace winrt::DittoWinUI::Services
{
    struct TrayIconService
    {
    public:
        TrayIconService();
        ~TrayIconService();

        bool Initialize(HWND hwnd);
        void Show();
        void Hide();
        void SetIcon(HICON icon);
        void SetTooltip(winrt::hstring tooltip);

        void ShowBalloon(winrt::hstring title, winrt::hstring text);

    private:
        NOTIFYICONDATA m_nid{};
        HWND m_hwnd{ nullptr };
        bool m_visible{ false };
    };
}
