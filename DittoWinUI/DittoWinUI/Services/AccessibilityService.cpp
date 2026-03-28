#include "IAccessibilityService.h"
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>

using namespace winrt::DittoWinUI::Services;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Automation;

AccessibilityService::AccessibilityService()
{
}

void AccessibilityService::Initialize(DependencyObject rootElement)
{
    m_rootElement = rootElement;
}

void AccessibilityService::SetFocusToFirstItem()
{
    if (m_rootElement)
    {
        auto focusManager = FocusManager::GetForCurrentView();
        if (focusManager)
        {
            auto firstElement = focusManager->FindFirstFocusableElement(m_rootElement);
            if (firstElement)
            {
                firstElement.Focus(FocusState::Keyboard);
            }
        }
    }
}

void AccessibilityService::SetFocusToSelectedItem()
{
    if (m_rootElement)
    {
        auto focusManager = FocusManager::GetForCurrentView();
        if (focusManager)
        {
            auto focusedElement = focusManager->GetFocusedElement();
            if (focusedElement)
            {
                focusedElement.Focus(FocusState::Keyboard);
            }
        }
    }
}

void AccessibilityService::MoveFocus(FocusNavigationDirection direction)
{
    auto focusManager = FocusManager::GetForCurrentView();
    if (focusManager)
    {
        FocusNavigationRequestArgs args(direction);
        focusManager->TryMoveFocus(direction, args);
    }
}

void AccessibilityService::AnnounceForAccessibility(winrt::hstring message)
{
    auto peer = Windows::UI::Xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(
        m_rootElement.as<Windows::UI::Xaml::FrameworkElement>());
    if (peer)
    {
        peer.RaiseNotificationEvent(
            AutomationNotificationKind::ItemAdded,
            AutomationNotificationProcessing::ImportantMostRecent,
            message,
            L"ClipboardUpdate");
    }
}

bool AccessibilityService::IsHighContrastEnabled() const
{
    auto accessibilitySettings = Windows::UI::ViewManagement::AccessibilitySettings();
    return accessibilitySettings.HighContrast();
}

void AccessibilityService::ApplyHighContrastTheme(bool enable)
{
    m_highContrastEnabled = enable;

    if (m_rootElement)
    {
        if (enable)
        {
            Application::Current().Resources().ThemeDictionaries().Insert(
                L"HighContrast",
                winrt::Windows::Foundation::Uri(L"ms-appx:///Resources/Themes/HighContrast.xaml"));
        }
        else
        {
            Application::Current().Resources().ThemeDictionaries().Remove(L"HighContrast");
        }
    }
}
