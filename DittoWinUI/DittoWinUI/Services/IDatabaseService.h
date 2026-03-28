#pragma once
#include <winrt/Windows.Foundation.h>
#include "ClipItem.h"

namespace winrt::DittoWinUI::Services
{
    struct IDatabaseService
    {
        virtual ~IDatabaseService() = default;

        virtual Windows::Foundation::IAsyncOperation<bool> InitializeAsync() = 0;
        virtual Windows::Foundation::IAsyncOperation<int64_t> InsertClipAsync(Models::ClipItem clip) = 0;
        virtual Windows::Foundation::IAsyncOperation<Models::ClipItem> GetClipByIdAsync(int64_t id) = 0;
        virtual Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Models::ClipItem>> QueryClipsAsync(Models::QueryFilter filter) = 0;
        virtual Windows::Foundation::IAsyncOperation<void> UpdateClipAsync(Models::ClipItem clip) = 0;
        virtual Windows::Foundation::IAsyncOperation<void> DeleteClipAsync(int64_t id) = 0;
        virtual Windows::Foundation::IAsyncOperation<int32_t> GetClipCountAsync() = 0;
        virtual Windows::Foundation::IAsyncOperation<void> PurgeOldClipsAsync(uint32_t keepCount) = 0;

        virtual Windows::Foundation::IAsyncOperation<int64_t> InsertGroupAsync(Models::ClipGroup group) = 0;
        virtual Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Models::ClipGroup>> GetAllGroupsAsync() = 0;
        virtual Windows::Foundation::IAsyncOperation<void> UpdateGroupAsync(Models::ClipGroup group) = 0;
        virtual Windows::Foundation::IAsyncOperation<void> DeleteGroupAsync(int32_t id) = 0;
    };
}
