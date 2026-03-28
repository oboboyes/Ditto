#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>

namespace winrt::DittoWinUI::Services
{
    struct IAccessibilityService
    {
        virtual ~IAccessibilityService() = default;

        virtual void SetFocusToFirstItem() = 0;
        virtual void SetFocusToSelectedItem() = 0;
        virtual void MoveFocus(FocusNavigationDirection direction) = 0;
        virtual void AnnounceForAccessibility(winrt::hstring message) = 0;
        virtual bool IsHighContrastEnabled() const = 0;
        virtual void ApplyHighContrastTheme(bool enable) = 0;
    };

    struct AccessibilityService : IAccessibilityService
    {
    public:
        AccessibilityService();

        void SetFocusToFirstItem() override;
        void SetFocusToSelectedItem() override;
        void MoveFocus(FocusNavigationDirection direction) override;
        void AnnounceForAccessibility(winrt::hstring message) override;
        bool IsHighContrastEnabled() const override;
        void ApplyHighContrastTheme(bool enable) override;

        void Initialize(Windows::UI::Xaml::DependencyObject rootElement);

    private:
        Windows::UI::Xaml::DependencyObject m_rootElement{ nullptr };
        bool m_highContrastEnabled{ false };
    };
}
