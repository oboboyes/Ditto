#include "SettingsDialog.xaml.h"

using namespace winrt::DittoWinUI::Views;
using namespace winrt::Windows::UI::Xaml::Controls;

SettingsDialog::SettingsDialog()
{
    InitializeComponent();
}

void SettingsDialog::SettingsNavList_SelectionChanged(ListView sender, ListViewSelectionChangedEventArgs args)
{
    auto selectedItem = sender.SelectedItem();
    if (selectedItem)
    {
        auto listViewItem = selectedItem.as<ListViewItem>();
        if (listViewItem)
        {
            auto tag = listViewItem.Tag();
            if (tag)
            {
                ShowSettingsPage(tag.as<winrt::hstring>());
            }
        }
    }
}

void SettingsDialog::RecordHotkey_Click(Windows::Foundation::IInspectable sender, Windows::UI::Xaml::RoutedEventArgs args)
{
}

void SettingsDialog::ShowSettingsPage(winrt::hstring pageName)
{
    GeneralSettings().Visibility(Windows::UI::Xaml::Visibility::Collapsed);
    HotkeysSettings().Visibility(Windows::UI::Xaml::Visibility::Collapsed);
    AppearanceSettings().Visibility(Windows::UI::Xaml::Visibility::Collapsed);
    StorageSettings().Visibility(Windows::UI::Xaml::Visibility::Collapsed);
    AboutSettings().Visibility(Windows::UI::Xaml::Visibility::Collapsed);

    if (pageName == L"General")
    {
        GeneralSettings().Visibility(Windows::UI::Xaml::Visibility::Visible);
    }
    else if (pageName == L"Hotkeys")
    {
        HotkeysSettings().Visibility(Windows::UI::Xaml::Visibility::Visible);
    }
    else if (pageName == L"Appearance")
    {
        AppearanceSettings().Visibility(Windows::UI::Xaml::Visibility::Visible);
    }
    else if (pageName == L"Storage")
    {
        StorageSettings().Visibility(Windows::UI::Xaml::Visibility::Visible);
    }
    else if (pageName == L"About")
    {
        AboutSettings().Visibility(Windows::UI::Xaml::Visibility::Visible);
    }
}

void SettingsDialog::LoadSettings()
{
}

void SettingsDialog::SaveSettings()
{
}
