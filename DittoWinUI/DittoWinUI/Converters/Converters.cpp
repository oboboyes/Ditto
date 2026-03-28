#include "Converters.h"

using namespace winrt::DittoWinUI::Converters;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Data;

namespace
{
    bool GetBoolValue(IInspectable value)
    {
        if (value)
        {
            auto box = value.as<Windows::Foundation::IPropertyValue>();
            if (box)
            {
                return box.GetBoolean();
            }
        }
        return false;
    }
}

IInspectable BoolToVisibilityConverter::Convert(
    IInspectable value,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    bool boolValue = GetBoolValue(value);
    return boolValue ? Visibility::Visible : Visibility::Collapsed;
}

IInspectable BoolToVisibilityConverter::ConvertBack(
    IInspectable value,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    auto visibility = value.as<Visibility>();
    return winrt::box_value(visibility == Visibility::Visible);
}

IInspectable InverseBoolConverter::Convert(
    IInspectable value,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    bool boolValue = GetBoolValue(value);
    return winrt::box_value(!boolValue);
}

IInspectable InverseBoolConverter::ConvertBack(
    IInspectable value,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    bool boolValue = GetBoolValue(value);
    return winrt::box_value(!boolValue);
}

IInspectable DateTimeConverter::Convert(
    IInspectable value,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    if (!value) return nullptr;

    try
    {
        auto dateTime = value.as<Windows::Foundation::DateTime>();
        auto duration = dateTime.UniversalTime();

        using namespace std::chrono;
        auto systemTime = time_point<system_clock, duration<long long, nano>>(
            duration<long long, nano>(duration));

        time_t time = system_clock::to_time_t(systemTime);
        tm localTime{};
        localtime_s(&localTime, &time);

        wchar_t buffer[64];
        wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d %H:%M", &localTime);

        return winrt::box_value(winrt::hstring(buffer));
    }
    catch (...)
    {
        return nullptr;
    }
}

IInspectable DateTimeConverter::ConvertBack(
    IInspectable,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    return nullptr;
}

IInspectable FileSizeConverter::Convert(
    IInspectable value,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    if (!value) return winrt::box_value(winrt::hstring(L"0 B"));

    try
    {
        auto box = value.as<Windows::Foundation::IPropertyValue>();
        if (box && box.Type() == Windows::Foundation::PropertyType::UInt64)
        {
            uint64_t size = box.GetUInt64();

            if (size < 1024)
            {
                return winrt::box_value(winrt::hstring(to_wstring(size) + L" B"));
            }
            else if (size < 1024 * 1024)
            {
                return winrt::box_value(winrt::hstring(to_wstring(size / 1024) + L" KB"));
            }
            else if (size < 1024 * 1024 * 1024)
            {
                return winrt::box_value(winrt::hstring(to_wstring(size / (1024 * 1024)) + L" MB"));
            }
            else
            {
                return winrt::box_value(winrt::hstring(to_wstring(size / (1024 * 1024 * 1024)) + L" GB"));
            }
        }
        return winrt::box_value(winrt::hstring(L"0 B"));
    }
    catch (...)
    {
        return winrt::box_value(winrt::hstring(L"0 B"));
    }
}

IInspectable FileSizeConverter::ConvertBack(
    IInspectable,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    return nullptr;
}

IInspectable NullToVisibilityConverter::Convert(
    IInspectable value,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    return value ? Visibility::Collapsed : Visibility::Visible;
}

IInspectable NullToVisibilityConverter::ConvertBack(
    IInspectable,
    Windows::UI::Xaml::Interop::TypeName,
    IInspectable,
    winrt::hstring)
{
    return nullptr;
}
