#include "TrayIconService.h"

namespace winrt::DittoWinUI::Services
{
    TrayIconService::TrayIconService()
    {
        memset(&m_nid, 0, sizeof(NOTIFYICONDATA));
    }

    TrayIconService::~TrayIconService()
    {
        if (m_hwnd && m_visible)
        {
            Shell_NotifyIcon(NIM_DELETE, &m_nid);
        }
    }

    bool TrayIconService::Initialize(HWND hwnd)
    {
        m_hwnd = hwnd;

        m_nid.cbSize = sizeof(NOTIFYICONDATA);
        m_nid.hWnd = hwnd;
        m_nid.uID = 1;
        m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        m_nid.uCallbackMessage = WM_USER + 1;
        m_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

        wcscpy_s(m_nid.szTip, L"Ditto WinUI 3");

        BOOL result = Shell_NotifyIcon(NIM_ADD, &m_nid);
        if (result)
        {
            m_visible = true;
        }

        return result != FALSE;
    }

    void TrayIconService::Show()
    {
        if (m_hwnd && !m_visible)
        {
            m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            Shell_NotifyIcon(NIM_ADD, &m_nid);
            m_visible = true;
        }
    }

    void TrayIconService::Hide()
    {
        if (m_hwnd && m_visible)
        {
            Shell_NotifyIcon(NIM_DELETE, &m_nid);
            m_visible = false;
        }
    }

    void TrayIconService::SetIcon(HICON icon)
    {
        if (m_hwnd)
        {
            m_nid.hIcon = icon;
            m_nid.uFlags = NIF_ICON;
            Shell_NotifyIcon(NIM_MODIFY, &m_nid);
        }
    }

    void TrayIconService::SetTooltip(winrt::hstring tooltip)
    {
        if (m_hwnd)
        {
            wcsncpy_s(m_nid.szTip, tooltip.c_str(), sizeof(m_nid.szTip) / sizeof(wchar_t) - 1);
            m_nid.uFlags = NIF_TIP;
            Shell_NotifyIcon(NIM_MODIFY, &m_nid);
        }
    }

    void TrayIconService::ShowBalloon(winrt::hstring title, winrt::hstring text)
    {
        if (m_hwnd && m_visible)
        {
            m_nid.uFlags = NIF_INFO;
            wcscpy_s(m_nid.szInfoTitle, title.c_str());
            wcscpy_s(m_nid.szInfo, text.c_str());
            m_nid.dwInfoFlags = NIIF_INFO;
            Shell_NotifyIcon(NIM_MODIFY, &m_nid);
        }
    }
}
