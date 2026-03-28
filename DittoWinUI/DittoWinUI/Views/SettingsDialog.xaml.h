#pragma once
#include "SettingsDialog.g.h"

namespace winrt::DittoWinUI::Views::implementation
{
    struct SettingsDialog : SettingsDialogT<SettingsDialog>
    {
        SettingsDialog();

        void SettingsNavList_SelectionChanged(Windows::UI::Xaml::Controls::ListView sender, 
            Windows::UI::Xaml::Controls::ListViewSelectionChangedEventArgs args);
        void RecordHotkey_Click(Windows::Foundation::IInspectable sender, 
            Windows::UI::Xaml::RoutedEventArgs args);

        void LoadSettings();
        void SaveSettings();

    private:
        void ShowSettingsPage(winrt::hstring pageName);
    };
}
