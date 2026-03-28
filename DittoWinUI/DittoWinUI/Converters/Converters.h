#pragma once
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt::DittoWinUI::Converters
{
    struct BoolToVisibilityConverter : winrt::Windows::UI::Xaml::Data::IValueConverter
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);
    };

    struct InverseBoolConverter : winrt::Windows::UI::Xaml::Data::IValueConverter
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);
    };

    struct DateTimeConverter : winrt::Windows::UI::Xaml::Data::IValueConverter
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);
    };

    struct FileSizeConverter : winrt::Windows::UI::Xaml::Data::IValueConverter
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);
    };

    struct NullToVisibilityConverter : winrt::Windows::UI::Xaml::Data::IValueConverter
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Windows::Foundation::IInspectable parameter,
            winrt::hstring language);
    };
}
