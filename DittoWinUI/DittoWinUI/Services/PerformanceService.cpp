#include "IPerformanceService.h"
#include <windows.h>
#include <psapi.h>
#include <algorithm>

using namespace winrt::DittoWinUI::Services;

PerformanceService::PerformanceService()
{
    m_timerResults = winrt::single_threaded_vector<TimerData>();
}

uint64_t PerformanceService::GetTimestamp()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

void PerformanceService::BeginTimer(winrt::hstring operationName)
{
    TimerData data;
    data.OperationName = operationName;
    data.StartTime = GetTimestamp();
    m_activeTimers.push_back(data);
}

void PerformanceService::EndTimer(winrt::hstring operationName)
{
    uint64_t endTime = GetTimestamp();
    for (auto& timer : m_activeTimers)
    {
        if (timer.OperationName == operationName)
        {
            timer.EndTime = endTime;
            m_timerResults.Append(timer);
            m_activeTimers.erase(std::remove(m_activeTimers.begin(), m_activeTimers.end(), timer), m_activeTimers.end());
            break;
        }
    }
}

uint64_t PerformanceService::GetMemoryUsage() const
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return pmc.WorkingSetSize;
    }
    return 0;
}

void PerformanceService::LogPerformanceMetrics()
{
    uint64_t memoryUsage = GetMemoryUsage();

    wchar_t buffer[256];
    swprintf_s(buffer, L"Memory Usage: %llu bytes (%.2f MB)", 
        memoryUsage, memoryUsage / (1024.0 * 1024.0));

    OutputDebugStringW(buffer);

    for (const auto& timer : m_timerResults)
    {
        swprintf_s(buffer, L"Timer [%s]: %llu ticks", 
            timer.OperationName.c_str(), timer.Duration());
        OutputDebugStringW(buffer);
    }
}

winrt::hstring DatabaseQueryOptimizer::OptimizeSearchQuery(winrt::hstring searchText)
{
    std::wstring query = searchText.c_str();

    std::replace(query.begin(), query.end(), L'\'', L'\'\'');

    while (query.find(L"  ") != std::wstring::npos)
    {
        size_t pos = query.find(L"  ");
        query.erase(pos, 1);
    }

    return winrt::hstring(query);
}

winrt::hstring DatabaseQueryOptimizer::BuildPaginatedQuery(winrt::hstring baseQuery, uint32_t pageIndex, uint32_t pageSize)
{
    uint32_t offset = pageIndex * pageSize;

    std::wstring query = baseQuery.c_str();
    query += L" LIMIT " + std::to_wstring(pageSize);
    query += L" OFFSET " + std::to_wstring(offset);

    return winrt::hstring(query);
}

bool DatabaseQueryOptimizer::UseIndexForQuery(const winrt::hstring& query)
{
    std::wstring q = query.c_str();

    std::transform(q.begin(), q.end(), q.begin(), ::towlower);

    if (q.find(L'where') != std::wstring::npos ||
        q.find(L'order by') != std::wstring::npos)
    {
        return true;
    }

    return false;
}

ClipboardCache::ClipboardCache(size_t maxSize)
    : m_maxSize(maxSize)
    , m_totalAccesses(0)
    , m_cacheHits(0)
{
}

ClipboardCache::~ClipboardCache()
{
}

void ClipboardCache::Put(int64_t key, Windows::Foundation::IInspectable value)
{
    if (m_cache.size() >= m_maxSize)
    {
        auto oldestIt = std::min_element(m_cache.begin(), m_cache.end(),
            [](const auto& a, const auto& b) {
                return a.second.AccessTime < b.second.AccessTime;
            });
        m_cache.erase(oldestIt);
    }

    CacheEntry entry;
    entry.Value = value;
    entry.AccessTime = GetTimestamp();
    entry.AccessCount = 1;

    m_cache[key] = entry;
}

Windows::Foundation::IInspectable ClipboardCache::Get(int64_t key)
{
    m_totalAccesses++;

    auto it = m_cache.find(key);
    if (it != m_cache.end())
    {
        m_cacheHits++;
        it->second.AccessTime = GetTimestamp();
        it->second.AccessCount++;
        return it->second.Value;
    }

    return nullptr;
}

void ClipboardCache::Clear()
{
    m_cache.clear();
    m_totalAccesses = 0;
    m_cacheHits = 0;
}

size_t ClipboardCache::Size() const
{
    return m_cache.size();
}

void ClipboardCache::SetMaxSize(size_t maxSize)
{
    m_maxSize = maxSize;
    while (m_cache.size() > m_maxSize)
    {
        auto oldestIt = std::min_element(m_cache.begin(), m_cache.end(),
            [](const auto& a, const auto& b) {
                return a.second.AccessTime < b.second.AccessTime;
            });
        m_cache.erase(oldestIt);
    }
}
