#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::DittoWinUI::Models
{
    struct QueryFilter
    {
        winrt::hstring SearchText;
        int32_t GroupId{ -1 };
        bool IncludePinnedOnly{ false };
        uint32_t MaxResults{ 100 };
        Windows::Foundation::DateTime CreatedAfter;
        Windows::Foundation::DateTime CreatedBefore;

        QueryFilter() = default;

        static QueryFilter Default()
        {
            QueryFilter filter;
            filter.MaxResults = 100;
            filter.GroupId = -1;
            return filter;
        }
    };

    struct ClipGroup
    {
        int32_t Id{ 0 };
        winrt::hstring Name;
        winrt::hstring Description;
        int32_t SortOrder{ 0 };
        bool IsExpanded{ true };

        ClipGroup() = default;
        explicit ClipGroup(int32_t id, winrt::hstring name) : Id(id), Name(name) {}
    };
}
