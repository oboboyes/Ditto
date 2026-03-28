#pragma once
#include "IClipboardService.h"
#include <thread>
#include <mutex>

namespace winrt::DittoWinUI::Services
{
    struct ClipboardService : IClipboardService
    {
    public:
        ClipboardService();

        Windows::Foundation::IAsyncOperation<bool> StartMonitoringAsync() override;
        Windows::Foundation::IAsyncOperation<void> StopMonitoringAsync() override;
        bool IsMonitoring() const override;

        Windows::Foundation::IAsyncOperation<Models::ClipItem> GetClipboardContentAsync() override;
        Windows::Foundation::IAsyncOperation<void> SetClipboardContentAsync(Models::ClipItem clip) override;
        Windows::Foundation::IAsyncOperation<void> ClearClipboardAsync() override;

    private:
        void ClipboardMonitoringLoop();
        Models::ClipItem GetClipboardContentInternal();

        bool m_monitoring;
        void* m_clipboardViewerHwnd;
        std::thread m_monitoringThread;
        std::mutex m_mutex;
    };
}
