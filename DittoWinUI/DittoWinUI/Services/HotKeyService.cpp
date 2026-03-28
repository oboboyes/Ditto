#include "HotKeyService.h"
#include <map>

namespace
{
    std::map<int32_t, HWND> g_hotKeyMap;
    HWND g_messageHwnd = nullptr;
}

namespace winrt::DittoWinUI::Services
{
    HotKeyService::HotKeyService()
    {
    }

    bool HotKeyService::RegisterHotKey(int32_t id, Models::HotKeyCombination combination)
    {
        HWND hwnd = Interop::GetMessageHwnd();
        if (!hwnd)
        {
            return false;
        }

        UINT modifiers = combination.ToWin32Modifiers();
        UINT vk = combination.VirtualKey;

        BOOL result = ::RegisterHotKey(hwnd, id, modifiers, vk);
        if (result)
        {
            g_hotKeyMap[id] = hwnd;
        }
        return result != 0;
    }

    bool HotKeyService::UnregisterHotKey(int32_t id)
    {
        auto it = g_hotKeyMap.find(id);
        if (it != g_hotKeyMap.end())
        {
            BOOL result = ::UnregisterHotKey(it->second, id);
            g_hotKeyMap.erase(it);
            return result != 0;
        }
        return false;
    }

    bool HotKeyService::UnregisterAll()
    {
        for (const auto& pair : g_hotKeyMap)
        {
            ::UnregisterHotKey(pair.second, pair.first);
        }
        g_hotKeyMap.clear();
        return true;
    }

    bool HotKeyService::IsHotKeyRegistered(int32_t id) const
    {
        return g_hotKeyMap.find(id) != g_hotKeyMap.end();
    }

    void HotKeyService::SetMessageHwnd(HWND hwnd)
    {
        Interop::SetMessageHwnd(hwnd);
    }

    void HotKeyService::ProcessHotKeyMessage(int32_t id)
    {
        if (m_hotKeyPressed)
        {
            m_hotKeyPressed(*this, id);
        }
    }
}

namespace winrt::DittoWinUI::Interop
{
    HWND GetMessageHwnd()
    {
        return g_messageHwnd;
    }

    void SetMessageHwnd(HWND hwnd)
    {
        g_messageHwnd = hwnd;
    }
}

namespace winrt::DittoWinUI::Models
{
    HotKeyCombination HotKeyCombination::Parse(winrt::hstring str)
    {
        HotKeyCombination combo;

        std::wstring s = str.c_str();
        size_t pos = 0;

        while (pos < s.size())
        {
            size_t nextPos = s.find(L'+', pos);
            std::wstring part = s.substr(pos, nextPos - pos);

            if (part == L"Ctrl" || part == L"Control")
                combo.Ctrl = true;
            else if (part == L"Alt")
                combo.Alt = true;
            else if (part == L"Shift")
                combo.Shift = true;
            else if (part == L"Win" || part == L"Windows")
                combo.Win = true;
            else if (part.size() == 1)
                combo.VirtualKey = toupper(part[0]);

            if (nextPos == std::wstring::npos)
                break;
            pos = nextPos + 1;
        }

        return combo;
    }

    UINT HotKeyCombination::ToWin32Modifiers() const
    {
        UINT modifiers = 0;
        if (Ctrl) modifiers |= MOD_CONTROL;
        if (Alt) modifiers |= MOD_ALT;
        if (Shift) modifiers |= MOD_SHIFT;
        if (Win) modifiers |= MOD_WIN;
        return modifiers;
    }

    winrt::hstring HotKeyCombination::ToString() const
    {
        std::wstring result;

        if (Ctrl) result += L"Ctrl+";
        if (Alt) result += L"Alt+";
        if (Shift) result += L"Shift+";
        if (Win) result += L"Win+";

        if (VirtualKey != 0)
        {
            wchar_t keyStr[2] = { (wchar_t)VirtualKey, 0 };
            result += keyStr;
        }

        return winrt::hstring(result);
    }
}
