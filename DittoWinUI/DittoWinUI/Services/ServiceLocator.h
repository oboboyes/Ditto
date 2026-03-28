#pragma once
#include <memory>
#include "Services/IDatabaseService.h"
#include "Services/ISettingsService.h"
#include "Services/IClipboardService.h"
#include "Services/IHotKeyService.h"

namespace winrt::DittoWinUI::Services
{
    struct ServiceLocator
    {
    public:
        static ServiceLocator& Instance()
        {
            static ServiceLocator instance;
            return instance;
        }

        IDatabaseService& Database() { return *m_databaseService; }
        ISettingsService& Settings() { return *m_settingsService; }
        IClipboardService& Clipboard() { return *m_clipboardService; }
        IHotKeyService& HotKey() { return *m_hotKeyService; }

        void SetDatabase(std::shared_ptr<IDatabaseService> service) { m_databaseService = service; }
        void SetSettings(std::shared_ptr<ISettingsService> service) { m_settingsService = service; }
        void SetClipboard(std::shared_ptr<IClipboardService> service) { m_clipboardService = service; }
        void SetHotKey(std::shared_ptr<IHotKeyService> service) { m_hotKeyService = service; }

    private:
        ServiceLocator() = default;

        std::shared_ptr<IDatabaseService> m_databaseService;
        std::shared_ptr<ISettingsService> m_settingsService;
        std::shared_ptr<IClipboardService> m_clipboardService;
        std::shared_ptr<IHotKeyService> m_hotKeyService;
    };
}
