#include "ClipboardService.h"
#include "ClipboardInterop.h"
#include <thread>
#include <chrono>

using namespace winrt::DittoWinUI::Services;
using namespace winrt::DittoWinUI::Models;

ClipboardService::ClipboardService()
    : m_monitoring(false)
    , m_clipboardViewerHwnd(nullptr)
{
}

Windows::Foundation::IAsyncOperation<bool> ClipboardService::StartMonitoringAsync()
{
    co_await winrt::resume_background();

    if (m_monitoring)
    {
        co_return true;
    }

    m_monitoring = true;
    m_monitoringThread = std::thread([this]() {
        ClipboardMonitoringLoop();
    });
    m_monitoringThread.detach();

    co_return true;
}

Windows::Foundation::IAsyncOperation<void> ClipboardService::StopMonitoringAsync()
{
    co_await winrt::resume_background();

    m_monitoring = false;

    if (m_clipboardViewerHwnd)
    {
        ClipboardInterop::RemoveClipboardViewer(m_clipboardViewerHwnd);
        m_clipboardViewerHwnd = nullptr;
    }

    co_return;
}

bool ClipboardService::IsMonitoring() const
{
    return m_monitoring;
}

void ClipboardService::ClipboardMonitoringLoop()
{
    while (m_monitoring)
    {
        if (m_clipboardChanged)
        {
            auto clip = GetClipboardContentInternal();
            if (clip.Formats.size() > 0)
            {
                m_clipboardChanged(*this, clip);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

ClipItem ClipboardService::GetClipboardContentInternal()
{
    ClipItem clip;
    clip.Created = Windows::Foundation::DateTime::Now();
    clip.Modified = clip.Created;

    HWND hwnd = GetClipboardViewerHwnd();
    if (!hwnd)
    {
        return clip;
    }

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
                    std::vector<uint8_t> rawData(sizeof(wchar_t) * (len + 1));
                    memcpy(rawData.data(), text, sizeof(wchar_t) * (len + 1));

                    ClipFormat clipFormat;
                    clipFormat.FormatId = CF_UNICODETEXT;
                    clipFormat.FormatName = L"Unicode Text";
                    clipFormat.RawData = rawData;
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
                    std::vector<uint8_t> rawData(len + 1);
                    memcpy(rawData.data(), text, len + 1);

                    ClipFormat clipFormat;
                    clipFormat.FormatId = CF_TEXT;
                    clipFormat.FormatName = L"Text";
                    clipFormat.RawData = rawData;
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

Windows::Foundation::IAsyncOperation<ClipItem> ClipboardService::GetClipboardContentAsync()
{
    co_await winrt::resume_background();
    co_return GetClipboardContentInternal();
}

Windows::Foundation::IAsyncOperation<void> ClipboardService::SetClipboardContentAsync(ClipItem clip)
{
    co_await winrt::resume_background();

    HWND hwnd = GetClipboardViewerHwnd();
    if (!hwnd)
    {
        co_return;
    }

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
                memcpy(pText, format.RawData.data(), format.RawData.size());
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
                memcpy(pText, format.RawData.data(), format.RawData.size());
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

    HWND hwnd = GetClipboardViewerHwnd();
    if (hwnd && ::OpenClipboard(hwnd))
    {
        ::EmptyClipboard();
        ::CloseClipboard();
    }

    co_return;
}
