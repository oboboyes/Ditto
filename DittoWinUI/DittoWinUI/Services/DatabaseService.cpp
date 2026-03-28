#include "DatabaseService.h"
#include <sqlite3.h>
#include <winrt/Windows.Storage.h>
#include <chrono>

using namespace winrt::DittoWinUI::Services;
using namespace winrt::DittoWinUI::Models;

namespace
{
    size_t HashSql(const wchar_t* sql)
    {
        size_t hash = 5381;
        while (*sql)
        {
            hash = ((hash << 5) + hash) + *sql++;
        }
        return hash;
    }
}

DatabaseService::DatabaseService(winrt::hstring dbPath)
    : m_dbPath(dbPath)
{
}

DatabaseService::~DatabaseService()
{
    ReleasePreparedStatements();
    
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
    
    if (m_dbReadonly)
    {
        sqlite3_close(m_dbReadonly);
        m_dbReadonly = nullptr;
    }
}

void DatabaseService::InitializeDatabaseOptimizations()
{
    const char* pragmas[] = {
        "PRAGMA journal_mode=WAL",
        "PRAGMA synchronous=NORMAL",
        "PRAGMA cache_size=-64000",
        "PRAGMA temp_store=MEMORY",
        "PRAGMA mmap_size=268435456",
        "PRAGMA page_size=4096",
        "PRAGMA read_uncommitted=1",
        "PRAGMA busy_timeout=5000"
    };

    for (const char* pragma : pragmas)
    {
        char* errMsg = nullptr;
        sqlite3_exec(m_db, pragma, nullptr, nullptr, &errMsg);
        if (errMsg)
        {
            sqlite3_free(errMsg);
        }
    }
}

void DatabaseService::InitializePreparedStatements()
{
    const wchar_t* statements[] = {
        L"SELECT lIndex, lClipID, lTime, bPin, lGroupID, Description FROM MainTable WHERE lIndex = ?;",
        L"SELECT lFormat FROM DataTable WHERE lClipID = ? LIMIT 1;",
        L"SELECT COUNT(*) FROM MainTable;",
        L"DELETE FROM DataTable WHERE lClipID = ?;",
        L"DELETE FROM MainTable WHERE lIndex = ?;"
    };

    for (const auto& sql : statements)
    {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, reinterpret_cast<const char*>(sql), -1, &stmt, nullptr) == SQLITE_OK)
        {
            size_t hash = HashSql(sql);
            m_statementCache[hash] = { stmt, winrt::hstring(sql), 0, 0 };
            sqlite3_finalize(stmt);
        }
    }
}

sqlite3_stmt* DatabaseService::GetPreparedStatement(const winrt::hstring& sql)
{
    size_t hash = HashSql(sql.c_str());
    
    std::lock_guard<std::mutex> lock(m_stmtMutex);
    
    auto it = m_statementCache.find(hash);
    if (it != m_statementCache.end())
    {
        it->second.lastUsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        it->second.useCount++;
        
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, reinterpret_cast<const char*>(sql.c_str()), -1, &stmt, nullptr) == SQLITE_OK)
        {
            return stmt;
        }
    }
    
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, reinterpret_cast<const char*>(sql.c_str()), -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (m_statementCache.size() >= MAX_CACHED_STATEMENTS)
        {
            uint64_t oldest = UINT64_MAX;
            auto oldestIt = m_statementCache.end();
            uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            
            for (auto it = m_statementCache.begin(); it != m_statementCache.end(); )
            {
                if (now - it->second.lastUsed > STMT_CACHE_TTL_MS)
                {
                    if (it->second.stmt)
                    {
                        sqlite3_finalize(it->second.stmt);
                    }
                    it = m_statementCache.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        
        m_statementCache[hash] = { stmt, sql, 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count(), 0 };
        return stmt;
    }
    
    return nullptr;
}

void DatabaseService::ReleasePreparedStatements()
{
    std::lock_guard<std::mutex> lock(m_stmtMutex);
    for (auto& pair : m_statementCache)
    {
        if (pair.second.stmt)
        {
            sqlite3_finalize(pair.second.stmt);
            pair.second.stmt = nullptr;
        }
    }
    m_statementCache.clear();
}

Windows::Foundation::IAsyncOperation<bool> DatabaseService::InitializeAsync()
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    int result = sqlite3_open16(m_dbPath.c_str(), &m_db);
    if (result != SQLITE_OK)
    {
        co_return false;
    }

    sqlite3_open16(m_dbPath.c_str(), &m_dbReadonly);

    InitializeDatabaseOptimizations();

    const char* createTableSql = 
        "CREATE TABLE IF NOT EXISTS MainTable ("
        "    lIndex INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    lClipParentID INTEGER DEFAULT 0,"
        "    lClipID INTEGER NOT NULL,"
        "    bIsChild INTEGER DEFAULT 0,"
        "    lTime DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    l便宜ID INTEGER DEFAULT 0,"
        "    bInGlobalViews INTEGER DEFAULT 0,"
        "    bPin INTEGER DEFAULT 0,"
        "    lGroupID INTEGER DEFAULT 0,"
        "    Description TEXT,"
        "    HitCount INTEGER DEFAULT 0"
        ");"

        "CREATE TABLE IF NOT EXISTS DataTable ("
        "    lIndex INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    lClipID INTEGER NOT NULL,"
        "    lFormat INTEGER NOT NULL,"
        "    BinaryData BLOB,"
        "    TextData TEXT,"
        "    FOREIGN KEY(lClipID) REFERENCES MainTable(lClipID)"
        ");"

        "CREATE TABLE IF NOT EXISTS GroupTable ("
        "    lGroupID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    Name TEXT NOT NULL,"
        "    Description TEXT,"
        "    SortOrder INTEGER DEFAULT 0"
        ");"

        "CREATE INDEX IF NOT EXISTS idx_clip_time ON MainTable(lTime DESC);"
        "CREATE INDEX IF NOT EXISTS idx_clip_group ON MainTable(lGroupID);"
        "CREATE INDEX IF NOT EXISTS idx_clip_pinned ON MainTable(bPin);"
        "CREATE INDEX IF NOT EXISTS idx_data_clipid ON DataTable(lClipID);"
        "CREATE INDEX IF NOT EXISTS idx_data_format ON DataTable(lFormat);";

    char* errMsg = nullptr;
    result = sqlite3_exec(m_db, createTableSql, nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK)
    {
        if (errMsg)
        {
            sqlite3_free(errMsg);
        }
        co_return false;
    }

    InitializePreparedStatements();

    co_return true;
}

Windows::Foundation::IAsyncOperation<void> DatabaseService::OptimizeDatabaseAsync()
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_exec(m_db, "VACUUM;", nullptr, nullptr, nullptr);
    sqlite3_exec(m_db, "ANALYZE;", nullptr, nullptr, nullptr);

    co_return;
}

Windows::Foundation::IAsyncOperation<int64_t> DatabaseService::InsertClipAsync(ClipItem clip)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    const char* sql = "INSERT INTO MainTable (lClipID, lTime, bPin, lGroupID, Description) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        co_return -1;
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    sqlite3_bind_int64(stmt, 1, clip.Id);
    sqlite3_bind_int64(stmt, 2, timestamp);
    sqlite3_bind_int(stmt, 3, clip.IsPinned ? 1 : 0);
    sqlite3_bind_int(stmt, 4, clip.GroupId);
    sqlite3_bind_text16(stmt, 5, clip.Description.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE)
    {
        co_return -1;
    }

    int64_t rowId = sqlite3_last_insert_rowid(m_db);

    for (const auto& format : clip.Formats)
    {
        InsertFormatData(rowId, format);
    }

    co_return rowId;
}

Windows::Foundation::IAsyncOperation<void> DatabaseService::BatchInsertClipsAsync(std::vector<ClipItem> clips)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char* sql = "INSERT INTO MainTable (lClipID, lTime, bPin, lGroupID, Description) VALUES (?, ?, ?, ?, ?);";
    
    auto now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    for (const auto& clip : clips)
    {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, clip.Id);
            sqlite3_bind_int64(stmt, 2, now);
            sqlite3_bind_int(stmt, 3, clip.IsPinned ? 1 : 0);
            sqlite3_bind_int(stmt, 4, clip.GroupId);
            sqlite3_bind_text16(stmt, 5, clip.Description.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);

            int64_t rowId = sqlite3_last_insert_rowid(m_db);
            for (const auto& format : clip.Formats)
            {
                InsertFormatData(rowId, format);
            }
        }
    }

    sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);

    co_return;
}

void DatabaseService::InsertFormatData(int64_t clipId, ClipFormat format)
{
    const char* sql = "INSERT INTO DataTable (lClipID, lFormat, BinaryData, TextData) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return;
    }

    sqlite3_bind_int64(stmt, 1, clipId);
    sqlite3_bind_int(stmt, 2, static_cast<int>(format.FormatId));

    if (!format.RawData.empty())
    {
        sqlite3_bind_blob(stmt, 3, format.RawData.data(), static_cast<int>(format.RawData.size()), SQLITE_TRANSIENT);
    }

    if (!format.FormatName.empty())
    {
        sqlite3_bind_text16(stmt, 4, format.FormatName.c_str(), -1, SQLITE_TRANSIENT);
    }

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

Windows::Foundation::IAsyncOperation<Models::ClipItem> DatabaseService::GetClipByIdAsync(int64_t id)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    const char* sql = "SELECT lClipID, lTime, bPin, lGroupID, Description FROM MainTable WHERE lIndex = ?;";
    sqlite3_stmt* stmt = nullptr;

    ClipItem clip;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        co_return clip;
    }

    sqlite3_bind_int64(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        clip.Id = sqlite3_column_int64(stmt, 0);

        auto timestamp = sqlite3_column_int64(stmt, 1);
        clip.Created = Windows::Foundation::DateTime{ std::chrono::duration_cast<Windows::Foundation::TimeSpan>(std::chrono::seconds(timestamp)) };
        clip.Modified = clip.Created;

        clip.IsPinned = sqlite3_column_int(stmt, 2) != 0;
        clip.GroupId = sqlite3_column_int(stmt, 3);

        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL)
        {
            clip.Description = winrt::hstring(reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 4)));
        }

        clip.Formats = GetFormatsForClip(clip.Id);
    }

    sqlite3_finalize(stmt);
    co_return clip;
}

std::vector<Models::ClipFormat> DatabaseService::GetFormatsForClip(int64_t clipId)
{
    std::vector<ClipFormat> formats;

    const char* sql = "SELECT lFormat, BinaryData, TextData FROM DataTable WHERE lClipID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return formats;
    }

    sqlite3_bind_int64(stmt, 1, clipId);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ClipFormat format;
        format.FormatId = sqlite3_column_int(stmt, 0);

        int blobSize = sqlite3_column_bytes(stmt, 1);
        if (blobSize > 0)
        {
            const void* blobData = sqlite3_column_blob(stmt, 1);
            format.RawData.assign(static_cast<const uint8_t*>(blobData), static_cast<const uint8_t*>(blobData) + blobSize);
        }

        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
        {
            format.FormatName = winrt::hstring(reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 2)));
        }

        formats.push_back(format);
    }

    sqlite3_finalize(stmt);
    return formats;
}

Models::ClipFormat DatabaseService::GetPrimaryFormatForClip(int64_t clipId)
{
    ClipFormat format;

    const char* sql = "SELECT lFormat, BinaryData, TextData FROM DataTable WHERE lClipID = ? LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return format;
    }

    sqlite3_bind_int64(stmt, 1, clipId);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        format.FormatId = sqlite3_column_int(stmt, 0);

        int blobSize = sqlite3_column_bytes(stmt, 1);
        if (blobSize > 0 && blobSize < 4096)
        {
            const void* blobData = sqlite3_column_blob(stmt, 1);
            format.RawData.assign(static_cast<const uint8_t*>(blobData), static_cast<const uint8_t*>(blobData) + blobSize);
        }

        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
        {
            format.FormatName = winrt::hstring(reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 2)));
        }
    }

    sqlite3_finalize(stmt);
    return format;
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Models::ClipItem>> DatabaseService::QueryClipsAsync(QueryFilter filter)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<ClipItem> results;
    results.reserve(std::min(filter.MaxResults, static_cast<uint32_t>(100)));

    std::wstring sql = L"SELECT lIndex, lClipID, lTime, bPin, lGroupID, Description FROM MainTable WHERE 1=1";

    if (filter.GroupId >= 0)
    {
        sql += L" AND lGroupID = " + std::to_wstring(filter.GroupId);
    }

    if (filter.IncludePinnedOnly)
    {
        sql += L" AND bPin = 1";
    }

    if (!filter.SearchText.empty())
    {
        sql += L" AND Description LIKE '%' || ? || '%'";
    }

    sql += L" ORDER BY bPin DESC, lTime DESC LIMIT " + std::to_wstring(filter.MaxResults);

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, reinterpret_cast<const char*>(sql.c_str()), -1, &stmt, nullptr) != SQLITE_OK)
    {
        co_return winrt::single_threaded_vector<ClipItem>();
    }

    if (!filter.SearchText.empty())
    {
        sqlite3_bind_text16(stmt, 1, filter.SearchText.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ClipItem clip;
        clip.Id = sqlite3_column_int64(stmt, 0);

        auto timestamp = sqlite3_column_int64(stmt, 2);
        clip.Created = Windows::Foundation::DateTime{ std::chrono::duration_cast<Windows::Foundation::TimeSpan>(std::chrono::seconds(timestamp)) };
        clip.Modified = clip.Created;

        clip.IsPinned = sqlite3_column_int(stmt, 3) != 0;
        clip.GroupId = sqlite3_column_int(stmt, 4);

        if (sqlite3_column_type(stmt, 5) != SQLITE_NULL)
        {
            clip.Description = winrt::hstring(reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 5)));
        }

        results.push_back(clip);
    }

    sqlite3_finalize(stmt);
    co_return winrt::single_threaded_vector<ClipItem>(std::move(results));
}

Windows::Foundation::IAsyncOperation<void> DatabaseService::UpdateClipAsync(ClipItem clip)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    const char* sql = "UPDATE MainTable SET bPin = ?, lGroupID = ?, Description = ? WHERE lIndex = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        co_return;
    }

    sqlite3_bind_int(stmt, 1, clip.IsPinned ? 1 : 0);
    sqlite3_bind_int(stmt, 2, clip.GroupId);
    sqlite3_bind_text16(stmt, 3, clip.Description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, clip.Id);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    co_return;
}

Windows::Foundation::IAsyncOperation<void> DatabaseService::DeleteClipAsync(int64_t id)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_exec(m_db, "BEGIN IMMEDIATE;", nullptr, nullptr, nullptr);

    const char* deleteDataSql = "DELETE FROM DataTable WHERE lClipID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, deleteDataSql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int64(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    const char* deleteMainSql = "DELETE FROM MainTable WHERE lIndex = ?;";
    if (sqlite3_prepare_v2(m_db, deleteMainSql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int64(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);

    co_return;
}

Windows::Foundation::IAsyncOperation<int32_t> DatabaseService::GetClipCountAsync()
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    const char* sql = "SELECT COUNT(*) FROM MainTable;";
    sqlite3_stmt* stmt = nullptr;

    int32_t count = 0;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    co_return count;
}

Windows::Foundation::IAsyncOperation<void> DatabaseService::PurgeOldClipsAsync(uint32_t keepCount)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_exec(m_db, "BEGIN IMMEDIATE;", nullptr, nullptr, nullptr);

    const char* getOldIdsSql = "SELECT lClipID FROM MainTable WHERE bPin = 0 ORDER BY lTime DESC LIMIT ?;";
    sqlite3_stmt* getStmt = nullptr;
    std::vector<int64_t> idsToDelete;

    if (sqlite3_prepare_v2(m_db, getOldIdsSql, -1, &getStmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int64(getStmt, 1, keepCount);
        while (sqlite3_step(getStmt) == SQLITE_ROW)
        {
            idsToDelete.push_back(sqlite3_column_int64(getStmt, 0));
        }
        sqlite3_finalize(getStmt);
    }

    const char* deleteDataSql = "DELETE FROM DataTable WHERE lClipID = ?;";
    const char* deleteMainSql = "DELETE FROM MainTable WHERE lIndex = ?;";

    sqlite3_stmt* delStmt = nullptr;

    for (int64_t id : idsToDelete)
    {
        if (sqlite3_prepare_v2(m_db, deleteDataSql, -1, &delStmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(delStmt, 1, id);
            sqlite3_step(delStmt);
            sqlite3_finalize(delStmt);
        }

        if (sqlite3_prepare_v2(m_db, deleteMainSql, -1, &delStmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(delStmt, 1, id);
            sqlite3_step(delStmt);
            sqlite3_finalize(delStmt);
        }
    }

    sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);

    co_return;
}

Windows::Foundation::IAsyncOperation<int64_t> DatabaseService::InsertGroupAsync(ClipGroup group)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    const char* sql = "INSERT INTO GroupTable (Name, Description, SortOrder) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        co_return -1;
    }

    sqlite3_bind_text16(stmt, 1, group.Name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 2, group.Description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, group.SortOrder);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    co_return sqlite3_last_insert_rowid(m_db);
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Models::ClipGroup>> DatabaseService::GetAllGroupsAsync()
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<ClipGroup> groups;

    const char* sql = "SELECT lGroupID, Name, Description, SortOrder FROM GroupTable ORDER BY SortOrder;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            ClipGroup group;
            group.Id = sqlite3_column_int(stmt, 0);
            group.Name = winrt::hstring(reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1)));
            group.Description = winrt::hstring(reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 2)));
            group.SortOrder = sqlite3_column_int(stmt, 3);
            groups.push_back(group);
        }
        sqlite3_finalize(stmt);
    }

    co_return winrt::single_threaded_vector<ClipGroup>(std::move(groups));
}

Windows::Foundation::IAsyncOperation<void> DatabaseService::UpdateGroupAsync(ClipGroup group)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    const char* sql = "UPDATE GroupTable SET Name = ?, Description = ?, SortOrder = ? WHERE lGroupID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text16(stmt, 1, group.Name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, 2, group.Description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, group.SortOrder);
        sqlite3_bind_int(stmt, 4, group.Id);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    co_return;
}

Windows::Foundation::IAsyncOperation<void> DatabaseService::DeleteGroupAsync(int32_t id)
{
    co_await winrt::resume_background();

    std::lock_guard<std::mutex> lock(m_mutex);

    const char* sql = "DELETE FROM GroupTable WHERE lGroupID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    co_return;
}
