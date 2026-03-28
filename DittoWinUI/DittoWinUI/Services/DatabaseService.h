#pragma once
#include "IDatabaseService.h"
#include <sqlite3.h>
#include <mutex>
#include <vector>
#include <unordered_map>

namespace winrt::DittoWinUI::Services
{
    struct DatabaseService : IDatabaseService
    {
    public:
        DatabaseService(winrt::hstring dbPath);
        ~DatabaseService();

        Windows::Foundation::IAsyncOperation<bool> InitializeAsync() override;
        Windows::Foundation::IAsyncOperation<int64_t> InsertClipAsync(Models::ClipItem clip) override;
        Windows::Foundation::IAsyncOperation<Models::ClipItem> GetClipByIdAsync(int64_t id) override;
        Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Models::ClipItem>> QueryClipsAsync(Models::QueryFilter filter) override;
        Windows::Foundation::IAsyncOperation<void> UpdateClipAsync(Models::ClipItem clip) override;
        Windows::Foundation::IAsyncOperation<void> DeleteClipAsync(int64_t id) override;
        Windows::Foundation::IAsyncOperation<int32_t> GetClipCountAsync() override;
        Windows::Foundation::IAsyncOperation<void> PurgeOldClipsAsync(uint32_t keepCount) override;

        Windows::Foundation::IAsyncOperation<int64_t> InsertGroupAsync(Models::ClipGroup group) override;
        Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Models::ClipGroup>> GetAllGroupsAsync() override;
        Windows::Foundation::IAsyncOperation<void> UpdateGroupAsync(Models::ClipGroup group) override;
        Windows::Foundation::IAsyncOperation<void> DeleteGroupAsync(int32_t id) override;

        Windows::Foundation::IAsyncOperation<void> BatchInsertClipsAsync(std::vector<Models::ClipItem> clips);
        Windows::Foundation::IAsyncOperation<void> OptimizeDatabaseAsync();

    private:
        struct PreparedStatement
        {
            sqlite3_stmt* stmt{ nullptr };
            winrt::hstring sql;
            uint64_t lastUsed{ 0 };
            uint32_t useCount{ 0 };
        };

        void InitializeDatabaseOptimizations();
        void InitializePreparedStatements();
        sqlite3_stmt* GetPreparedStatement(const winrt::hstring& sql);
        void ReleasePreparedStatements();
        
        void InsertFormatData(int64_t clipId, Models::ClipFormat format);
        std::vector<Models::ClipFormat> GetFormatsForClip(int64_t clipId);
        Models::ClipFormat GetPrimaryFormatForClip(int64_t clipId);

        winrt::hstring m_dbPath;
        sqlite3* m_db{ nullptr };
        sqlite3* m_dbReadonly{ nullptr };
        std::mutex m_mutex;
        std::mutex m_stmtMutex;
        std::unordered_map<size_t, PreparedStatement> m_statementCache;
        static constexpr size_t MAX_CACHED_STATEMENTS = 20;
        static constexpr uint64_t STMT_CACHE_TTL_MS = 60000;
    };
}
