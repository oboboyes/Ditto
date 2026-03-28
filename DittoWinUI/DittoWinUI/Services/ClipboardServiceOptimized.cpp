#include "ClipboardServiceOptimized.h"
#include <zlib.h>
#include <algorithm>

using namespace winrt::DittoWinUI::Services;

ClipboardService::ClipboardService()
{
}

Windows::Foundation::IAsyncOperation<bool> ClipboardService::StartMonitoringAsync()
{
    co_await winrt::resume_background();

    if (m_monitoring.load())
    {
        co_return true;
    }

    m_monitoring = true;
    m_changeCount = 0;

    m_monitoringThread = std::thread([this]() {
        ClipboardMonitoringLoop();
    });

    co_return true;
}

Windows::Foundation::IAsyncOperation<void> ClipboardService::StopMonitoringAsync()
{
    co_await winrt::resume_background();

    m_monitoring = false;

    if (m_monitoringThread.joinable())
    {
        m_monitoringThread.join();
    }

    ReleaseClipboardResources();

    co_return;
}

void ClipboardService::SetDebounceInterval(int32_t milliseconds)
{
    m_debounceIntervalMs = std::max(10, std::min(milliseconds, 1000));
}

void ClipboardService::SetChangeDetectionCallback(std::function<void()> callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_changeCallback = std::move(callback);
}

void ClipboardService::ClipboardMonitoringLoop()
{
    ClipboardChangeDetector detector;

    while (m_monitoring.load())
    {
        if (DetectClipboardChange())
        {
            m_changeCount++;

            if (m_changeCallback)
            {
                m_changeCallback();
            }

            if (m_clipboardChanged)
            {
                auto clip = GetClipboardContentInternal();
                if (clip.Formats.size() > 0)
                {
                    m_clipboardChanged(*this, clip);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(m_debounceIntervalMs));
    }
}

bool ClipboardService::DetectClipboardChange()
{
    static ClipboardChangeDetector detector;

    HWND hwnd = Interop::GetClipboardViewerHwnd();
    if (!hwnd)
    {
        return false;
    }

    if (!::OpenClipboard(hwnd))
    {
        return false;
    }

    HANDLE clipboardData = ::GetClipboardData(CF_UNICODETEXT);
    uint32_t currentFormat = CF_UNICODETEXT;

    if (!clipboardData)
    {
        clipboardData = ::GetClipboardData(CF_TEXT);
        currentFormat = CF_TEXT;
    }

    bool changed = detector.HasClipboardChanged(clipboardData, currentFormat);

    if (!changed)
    {
        clipboardData = ::GetClipboardData(CF_BITMAP);
        if (clipboardData)
        {
            changed = detector.HasClipboardChanged(clipboardData, CF_BITMAP);
            if (changed) currentFormat = CF_BITMAP;
        }
    }

    if (!changed)
    {
        clipboardData = ::GetClipboardData(CF_HDROP);
        if (clipboardData)
        {
            changed = detector.HasClipboardChanged(clipboardData, CF_HDROP);
            if (changed) currentFormat = CF_HDROP;
        }
    }

    ::CloseClipboard();

    return changed;
}

Models::ClipItem ClipboardService::GetClipboardContentInternal()
{
    ClipItem clip;
    clip.Created = Windows::Foundation::DateTime::Now();
    clip.Modified = clip.Created;

    HWND hwnd = Interop::GetClipboardViewerHwnd();
    if (!hwnd)
    {
        return clip;
    }

    std::lock_guard<std::mutex> lock(m_clipboardMutex);

    if (!::OpenClipboard(hwnd))
    {
        return clip;
    }

    UINT format = 0;
    while ((format = ::EnumClipboardFormats(format)) != 0)
    {
        if (format == CF_UNICODETEXT)
        {
            HANDLE data = ::GetClipboardData(CF_UNICODETEXT);
            if (data)
            {
                wchar_t* text = static_cast<wchar_t*>(::GlobalLock(data));
                if (text)
                {
                    size_t len = wcslen(text);
                    size_t dataSize = sizeof(wchar_t) * (len + 1);

                    ClipFormat clipFormat;
                    clipFormat.FormatId = CF_UNICODETEXT;
                    clipFormat.FormatName = L"Unicode Text";

                    if (LargeDataHandler::ShouldCompress(dataSize))
                    {
                        clipFormat.RawData = LargeDataHandler::CompressData(
                            reinterpret_cast<const uint8_t*>(text), dataSize);
                    }
                    else
                    {
                        clipFormat.RawData.resize(dataSize);
                        memcpy(clipFormat.RawData.data(), text, dataSize);
                    }

                    clip.Formats.push_back(clipFormat);
                    clip.Description = winrt::hstring(text);

                    ::GlobalUnlock(data);
                }
            }
        }
        else if (format == CF_TEXT)
        {
            HANDLE data = ::GetClipboardData(CF_TEXT);
            if (data)
            {
                char* text = static_cast<char*>(::GlobalLock(data));
                if (text)
                {
                    size_t len = strlen(text);
                    ClipFormat clipFormat;
                    clipFormat.FormatId = CF_TEXT;
                    clipFormat.FormatName = L"Text";
                    clipFormat.RawData.assign(reinterpret_cast<uint8_t*>(text), reinterpret_cast<uint8_t*>(text) + len + 1);
                    clip.Formats.push_back(clipFormat);
                    ::GlobalUnlock(data);
                }
            }
        }
        else if (format == CF_BITMAP)
        {
            HANDLE data = ::GetClipboardData(CF_BITMAP);
            if (data)
            {
                ClipFormat clipFormat;
                clipFormat.FormatId = CF_BITMAP;
                clipFormat.FormatName = L"Bitmap";
                clip.Formats.push_back(clipFormat);
            }
        }
        else if (format == CF_HDROP)
        {
            HANDLE data = ::GetClipboardData(CF_HDROP);
            if (data)
            {
                HDROP hdrop = static_cast<HDROP>(data);
                UINT fileCount = ::DragQueryFile(hdrop, 0xFFFFFFFF, nullptr, 0);

                std::wstring files;
                for (UINT i = 0; i < fileCount; ++i)
                {
                    wchar_t filePath[MAX_PATH];
                    if (::DragQueryFile(hdrop, i, filePath, MAX_PATH))
                    {
                        if (i > 0) files += L"\n";
                        files += filePath;
                    }
                }

                if (!files.empty())
                {
                    ClipFormat clipFormat;
                    clipFormat.FormatId = CF_HDROP;
                    clipFormat.FormatName = L"File Drop";
                    clipFormat.RawData.assign((uint8_t*)files.c_str(), (uint8_t*)(files.c_str() + files.size() + 1));
                    clip.Formats.push_back(clipFormat);
                    clip.Description = L"[Files]";
                }
            }
        }
    }

    ::CloseClipboard();
    return clip;
}

Windows::Foundation::IAsyncOperation<Models::ClipItem> ClipboardService::GetClipboardContentAsync()
{
    co_await winrt::resume_background();
    co_return GetClipboardContentInternal();
}

Windows::Foundation::IAsyncOperation<void> ClipboardService::SetClipboardContentAsync(ClipItem clip)
{
    co_await winrt::resume_background();

    HWND hwnd = Interop::GetClipboardViewerHwnd();
    if (!hwnd)
    {
        co_return;
    }

    std::lock_guard<std::mutex> lock(m_clipboardMutex);

    if (!::OpenClipboard(hwnd))
    {
        co_return;
    }

    ::EmptyClipboard();

    for (const auto& format : clip.Formats)
    {
        if (format.FormatId == CF_UNICODETEXT && !format.RawData.empty())
        {
            size_t charCount = format.RawData.size() / sizeof(wchar_t);
            HGLOBAL hMem = ::GlobalAlloc(GHND, format.RawData.size());
            if (hMem)
            {
                wchar_t* pText = static_cast<wchar_t*>(::GlobalLock(hMem));
                if (pText)
                {
                    if (LargeDataHandler::ShouldCompress(format.RawData.size()))
                    {
                        auto decompressed = LargeDataHandler::DecompressData(
                            format.RawData.data(), 
                            format.RawData.size(),
                            sizeof(wchar_t) * charCount);
                        if (!decompressed.empty())
                        {
                            memcpy(pText, decompressed.data(), decompressed.size());
                        }
                    }
                    else
                    {
                        memcpy(pText, format.RawData.data(), format.RawData.size());
                    }
                }
                ::GlobalUnlock(hMem);
                ::SetClipboardData(CF_UNICODETEXT, hMem);
            }
        }
        else if (format.FormatId == CF_TEXT && !format.RawData.empty())
        {
            HGLOBAL hMem = ::GlobalAlloc(GHND, format.RawData.size());
            if (hMem)
            {
                char* pText = static_cast<char*>(::GlobalLock(hMem));
                if (pText)
                {
                    memcpy(pText, format.RawData.data(), format.RawData.size());
                }
                ::GlobalUnlock(hMem);
                ::SetClipboardData(CF_TEXT, hMem);
            }
        }
    }

    ::CloseClipboard();
    co_return;
}

Windows::Foundation::IAsyncOperation<void> ClipboardService::ClearClipboardAsync()
{
    co_await winrt::resume_background();

    HWND hwnd = Interop::GetClipboardViewerHwnd();
    if (hwnd && ::OpenClipboard(hwnd))
    {
        ::EmptyClipboard();
        ::CloseClipboard();
    }

    ReleaseClipboardResources();

    co_return;
}

void ClipboardService::ReleaseClipboardResources()
{
    std::lock_guard<std::mutex> lock(m_clipboardMutex);
    m_lastClipboardData = nullptr;
    m_lastClipboardFormat = 0;
}

ClipboardChangeDetector::ClipboardChangeDetector()
{
}

bool ClipboardChangeDetector::HasClipboardChanged(HANDLE clipboardData, uint32_t format)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (clipboardData != m_lastHandle || format != m_lastFormat)
    {
        return true;
    }

    return false;
}

void ClipboardChangeDetector::UpdateLastClipboardState(HANDLE clipboardData, uint32_t format)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastHandle = clipboardData;
    m_lastFormat = format;
}

void ClipboardChangeDetector::Reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastHandle = nullptr;
    m_lastFormat = 0;
    m_lastCheckSum = 0;
}

bool LargeDataHandler::ShouldCompress(size_t dataSize, size_t thresholdKB)
{
    return dataSize > thresholdKB * 1024;
}

std::vector<uint8_t> LargeDataHandler::CompressData(const uint8_t* data, size_t size)
{
    if (size == 0) return {};

    uLongf compressedSize = compressBound(size);
    std::vector<uint8_t> compressed(compressedSize + sizeof(size));

    if (compress2(compressed.data() + sizeof(size), &compressedSize, data, size, Z_DEFAULT_COMPRESSION) != Z_OK)
    {
        return {};
    }

    memcpy(compressed.data(), &size, sizeof(size));
    compressed.resize(compressedSize + sizeof(size));

    return compressed;
}

std::vector<uint8_t> LargeDataHandler::DecompressData(const uint8_t* data, size_t compressedSize, size_t uncompressedSize)
{
    if (compressedSize == 0 || uncompressedSize == 0) return {};

    std::vector<uint8_t> uncompressed(uncompressedSize);
    uLongf destLen = uncompressedSize;

    size_t originalSize = 0;
    memcpy(&originalSize, data, sizeof(size_t));

    if (uncompress2(uncompressed.data(), &destLen, data + sizeof(size_t), compressedSize - sizeof(size_t)) != Z_OK)
    {
        return {};
    }

    return uncompressed;
}
