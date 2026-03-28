#pragma once
#include <winrt/Windows.Foundation.h>

namespace winrt::DittoWinUI::Models
{
    struct HotKeyCombination
    {
        bool Ctrl{ false };
        bool Alt{ false };
        bool Shift{ false };
        bool Win{ false };
        uint32_t VirtualKey{ 0 };

        HotKeyCombination() = default;
        HotKeyCombination(uint32_t vk) : VirtualKey(vk) {}

        static HotKeyCombination Parse(winrt::hstring str);

        uint32_t ToWin32Modifiers() const;
        winrt::hstring ToString() const;

        bool operator==(const HotKeyCombination& other) const
        {
            return Ctrl == other.Ctrl && Alt == other.Alt && Shift == other.Shift &&
                   Win == other.Win && VirtualKey == other.VirtualKey;
        }
    };
}

namespace winrt::DittoWinUI::Services
{
    struct IHotKeyService
    {
        virtual ~IHotKeyService() = default;

        virtual bool RegisterHotKey(int32_t id, Models::HotKeyCombination combination) = 0;
        virtual bool UnregisterHotKey(int32_t id) = 0;
        virtual bool UnregisterAll() = 0;
        virtual bool IsHotKeyRegistered(int32_t id) const = 0;

        event_token HotKeyPressed(Windows::Foundation::EventHandler<int32_t> handler);
        void RemoveHotKeyPressed(event_token token);

    protected:
        event<Windows::Foundation::EventHandler<int32_t>> m_hotKeyPressed;
    };

    inline event_token IHotKeyService::HotKeyPressed(Windows::Foundation::EventHandler<int32_t> handler)
    {
        return m_hotKeyPressed.add(handler);
    }

    inline void IHotKeyService::RemoveHotKeyPressed(event_token token)
    {
        m_hotKeyPressed.remove(token);
    }
}
