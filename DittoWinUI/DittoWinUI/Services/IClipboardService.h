#pragma once
#include <winrt/Windows.Foundation.h>
#include "Models/ClipItem.h"

namespace winrt::DittoWinUI::Services
{
    struct IClipboardService
    {
        virtual ~IClipboardService() = default;

        virtual Windows::Foundation::IAsyncOperation<bool> StartMonitoringAsync() = 0;
        virtual Windows::Foundation::IAsyncOperation<void> StopMonitoringAsync() = 0;
        virtual bool IsMonitoring() const = 0;

        virtual Windows::Foundation::IAsyncOperation<Models::ClipItem> GetClipboardContentAsync() = 0;
        virtual Windows::Foundation::IAsyncOperation<void> SetClipboardContentAsync(Models::ClipItem clip) = 0;
        virtual Windows::Foundation::IAsyncOperation<void> ClearClipboardAsync() = 0;

        event_token ClipboardChanged(Windows::Foundation::EventHandler<Models::ClipItem> handler);
        void RemoveClipboardChanged(event_token token);

    protected:
        event<Windows::Foundation::EventHandler<Models::ClipItem>> m_clipboardChanged;
    };

    inline event_token IClipboardService::ClipboardChanged(Windows::Foundation::EventHandler<Models::ClipItem> handler)
    {
        return m_clipboardChanged.add(handler);
    }

    inline void IClipboardService::RemoveClipboardChanged(event_token token)
    {
        m_clipboardChanged.remove(token);
    }
}
