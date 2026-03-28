#pragma once
#include "IHotKeyService.h"
#include <map>

namespace winrt::DittoWinUI::Interop
{
    HWND GetMessageHwnd();
    void SetMessageHwnd(HWND hwnd);
}

namespace winrt::DittoWinUI::Services
{
    struct HotKeyService : IHotKeyService
    {
    public:
        HotKeyService();

        bool RegisterHotKey(int32_t id, Models::HotKeyCombination combination) override;
        bool UnregisterHotKey(int32_t id) override;
        bool UnregisterAll() override;
        bool IsHotKeyRegistered(int32_t id) const override;

        void SetMessageHwnd(HWND hwnd);
        void ProcessHotKeyMessage(int32_t id);

    private:
        std::map<int32_t, HWND> m_hotKeyMap;
    };
}
