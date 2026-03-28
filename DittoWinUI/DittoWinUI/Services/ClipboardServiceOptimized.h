#pragma once
#include "IClipboardService.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <functional>

namespace winrt::DittoWinUI::Services
{
    struct ClipboardService : IClipboardService
    {
    public:
        ClipboardService();

        Windows::Foundation::IAsyncOperation<bool> StartMonitoringAsync() override;
        Windows::Foundation::IAsyncOperation<void> StopMonitoringAsync() override;
        bool IsMonitoring() const override { return m_monitoring.load(); }

        Windows::Foundation::IAsyncOperation<Models::ClipItem> GetClipboardContentAsync() override;
        Windows::Foundation::IAsyncOperation<void> SetClipboardContentAsync(Models::ClipItem clip) override;
        Windows::Foundation::IAsyncOperation<void> ClearClipboardAsync() override;

        void SetDebounceInterval(int32_t milliseconds);
        void SetChangeDetectionCallback(std::function<void()> callback);

    private:
        void ClipboardMonitoringLoop();
        bool DetectClipboardChange();
        Models::ClipItem GetClipboardContentInternal();
        void ReleaseClipboardResources();

        std::atomic<bool> m_monitoring{ false };
        std::atomic<bool> m_clipboardChanged{ false };
        std::thread m_monitoringThread;
        std::mutex m_mutex;
        std::mutex m_clipboardMutex;
        
        void* m_lastClipboardData{ nullptr };
        uint32_t m_lastClipboardFormat{ 0 };
        int32_t m_debounceIntervalMs{ 100 };
        
        std::function<void()> m_changeCallback;
        std::atomic<uint64_t> m_changeCount{ 0 };
    };

    struct ClipboardMonitorConfig
    {
        int32_t PollingIntervalMs{ 100 };
        int32_t DebounceIntervalMs{ 50 };
        uint32_t MaxMemoryPerClipKB{ 4096 };
        bool EnableChangeDetection{ true };
        bool EnableDeduplication{ true };
    };

    class ClipboardChangeDetector
    {
    public:
        ClipboardChangeDetector();

        bool HasClipboardChanged(HANDLE clipboardData, uint32_t format);
        void UpdateLastClipboardState(HANDLE clipboardData, uint32_t format);
        void Reset();

    private:
        HANDLE m_lastHandle{ nullptr };
        uint32_t m_lastFormat{ 0 };
        uint64_t m_lastCheckSum{ 0 };
        std::mutex m_mutex;
    };

    struct LargeDataHandler
    {
        static bool ShouldCompress(size_t dataSize, size_t thresholdKB = 64);
        static std::vector<uint8_t> CompressData(const uint8_t* data, size_t size);
        static std::vector<uint8_t> DecompressData(const uint8_t* data, size_t compressedSize, size_t uncompressedSize);
    };
}
