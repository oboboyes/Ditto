#pragma once
#include <windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <memory>

namespace winrt::DittoWinUI::Services
{
    template<typename T>
    struct AsyncOperationResult
    {
        bool Success;
        T Result;
        winrt::hstring ErrorMessage;

        static AsyncOperationResult SuccessResult(T result)
        {
            AsyncOperationResult ar;
            ar.Success = true;
            ar.Result = result;
            return ar;
        }

        static AsyncOperationResult ErrorResult(winrt::hstring error)
        {
            AsyncOperationResult ar;
            ar.Success = false;
            ar.ErrorMessage = error;
            return ar;
        }
    };

    struct IPerformanceService
    {
        virtual ~IPerformanceService() = default;

        virtual void BeginTimer(winrt::hstring operationName) = 0;
        virtual void EndTimer(winrt::hstring operationName) = 0;
        virtual uint64_t GetMemoryUsage() const = 0;
        virtual void LogPerformanceMetrics() = 0;
    };

    struct PerformanceService : IPerformanceService
    {
    public:
        PerformanceService();

        void BeginTimer(winrt::hstring operationName) override;
        void EndTimer(winrt::hstring operationName) override;
        uint64_t GetMemoryUsage() const override;
        void LogPerformanceMetrics() override;

        struct TimerData
        {
            winrt::hstring OperationName;
            uint64_t StartTime;
            uint64_t EndTime;
            uint64_t Duration() const { return EndTime - StartTime; }
        };

        Windows::Foundation::Collections::IVector<TimerData> GetTimerResults() { return m_timerResults; }

    private:
        uint64_t GetTimestamp();
        uint64_t GetProcessMemory();

        Windows::Foundation::Collections::IVector<TimerData> m_timerResults;
        std::vector<TimerData> m_activeTimers;
    };

    struct DatabaseQueryOptimizer
    {
        static winrt::hstring OptimizeSearchQuery(winrt::hstring searchText);
        static winrt::hstring BuildPaginatedQuery(winrt::hstring baseQuery, uint32_t pageIndex, uint32_t pageSize);
        static bool UseIndexForQuery(const winrt::hstring& query);
    };

    struct ClipboardCache
    {
        ClipboardCache(size_t maxSize = 100);
        ~ClipboardCache();

        void Put(int64_t key, Windows::Foundation::IInspectable value);
        Windows::Foundation::IInspectable Get(int64_t key);
        void Clear();
        size_t Size() const;
        void SetMaxSize(size_t maxSize);

    private:
        struct CacheEntry
        {
            Windows::Foundation::IInspectable Value;
            uint64_t AccessTime;
            uint32_t AccessCount;
        };

        std::map<int64_t, CacheEntry> m_cache;
        size_t m_maxSize;
        uint64_t m_totalAccesses;
        uint64_t m_cacheHits;
    };
}
