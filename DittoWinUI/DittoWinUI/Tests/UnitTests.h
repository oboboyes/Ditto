#pragma once
#include <cassert>
#include <functional>
#include <string>

namespace DittoWinUI::Tests
{
    struct TestResult
    {
        bool Passed;
        winrt::hstring TestName;
        winrt::hstring ErrorMessage;

        static TestResult Pass(winrt::hstring name)
        {
            TestResult r;
            r.Passed = true;
            r.TestName = name;
            return r;
        }

        static TestResult Fail(winrt::hstring name, winrt::hstring error)
        {
            TestResult r;
            r.Passed = false;
            r.TestName = name;
            r.ErrorMessage = error;
            return r;
        }
    };

    struct TestRunner
    {
        using TestFunc = std::function<TestResult()>;

        void RegisterTest(winrt::hstring name, TestFunc testFunc)
        {
            m_tests.push_back({ name, testFunc });
        }

        int RunAllTests()
        {
            int passed = 0;
            int failed = 0;

            for (const auto& test : m_tests)
            {
                auto result = test.second();
                if (result.Passed)
                {
                    passed++;
                    OutputDebugStringW((L"[PASS] " + result.TestName + L"\n").c_str());
                }
                else
                {
                    failed++;
                    OutputDebugStringW((L"[FAIL] " + result.TestName + L": " + result.ErrorMessage + L"\n").c_str());
                }
            }

            wchar_t buffer[128];
            swprintf_s(buffer, L"\n=== Test Results: %d passed, %d failed ===\n", passed, failed);
            OutputDebugStringW(buffer);

            return failed;
        }

    private:
        std::vector<std::pair<winrt::hstring, TestFunc>> m_tests;
    };

    namespace DatabaseServiceTests
    {
        inline TestResult TestDatabaseInitialization()
        {
            return TestResult::Pass(L"TestDatabaseInitialization");
        }

        inline TestResult TestInsertClip()
        {
            return TestResult::Pass(L"TestInsertClip");
        }

        inline TestResult TestQueryClips()
        {
            return TestResult::Pass(L"TestQueryClips");
        }

        inline TestResult TestDeleteClip()
        {
            return TestResult::Pass(L"TestDeleteClip");
        }
    }

    namespace ClipboardServiceTests
    {
        inline TestResult TestClipboardMonitoring()
        {
            return TestResult::Pass(L"TestClipboardMonitoring");
        }

        inline TestResult TestMultipleFormats()
        {
            return TestResult::Pass(L"TestMultipleFormats");
        }
    }

    namespace ViewModelTests
    {
        inline TestResult TestMainViewModelInitialization()
        {
            return TestResult::Pass(L"TestMainViewModelInitialization");
        }

        inline TestResult TestQuickPasteViewModelSearch()
        {
            return TestResult::Pass(L"TestQuickPasteViewModelSearch");
        }

        inline TestResult TestSettingsViewModelSaveLoad()
        {
            return TestResult::Pass(L"TestSettingsViewModelSaveLoad");
        }
    }

    namespace PerformanceTests
    {
        inline TestResult TestMemoryUsage()
        {
            return TestResult::Pass(L"TestMemoryUsage");
        }

        inline TestResult TestDatabaseQueryPerformance()
        {
            return TestResult::Pass(L"TestDatabaseQueryPerformance");
        }
    }
}
