#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <vector>

namespace winrt::DittoWinUI::Models
{
    struct ClipFormat
    {
        uint32_t FormatId{ 0 };
        winrt::hstring FormatName;
        std::vector<uint8_t> RawData;
        Windows::Storage::Streams::IBuffer Buffer{ nullptr };

        ClipFormat() = default;
        ClipFormat(uint32_t id, winrt::hstring name) : FormatId(id), FormatName(name) {}
    };

    struct ClipItem
    {
        int64_t Id{ 0 };
        std::vector<ClipFormat> Formats;
        Windows::Foundation::DateTime Created;
        Windows::Foundation::DateTime Modified;
        int32_t GroupId{ 0 };
        bool IsPinned{ false };
        winrt::hstring Description;

        ClipItem() = default;
        explicit ClipItem(int64_t id) : Id(id) {}

        Windows::Foundation::DateTime GetCreatedTime() const { return Created; }
        void SetCreatedTime(Windows::Foundation::DateTime value) { Created = value; }

        Windows::Foundation::DateTime GetModifiedTime() const { return Modified; }
        void SetModifiedTime(Windows::Foundation::DateTime value) { Modified = value; }

        bool GetIsPinned() const { return IsPinned; }
        void SetIsPinned(bool value) { IsPinned = value; }

        int32_t GetGroupId() const { return GroupId; }
        void SetGroupId(int32_t value) { GroupId = value; }
    };
}
