#include "QuickPasteWindow.xaml.h"

using namespace winrt::DittoWinUI::Views;
using namespace winrt::Windows::UI::Xaml::Controls;

QuickPasteWindow::QuickPasteWindow()
{
    InitializeComponent();
}

void QuickPasteWindow::SearchBox_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
{
    if (args.Reason() == AutoSuggestionBoxTextChangeReason::UserInput)
    {
        FilterClips(sender.Text());
    }
}

void QuickPasteWindow::SearchBox_QuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
{
    FilterClips(sender.Text());
}

void QuickPasteWindow::ClipListView_SelectionChanged(ListView sender, ListViewSelectionChangedEventArgs args)
{
}

void QuickPasteWindow::ClipListView_ItemClick(ListView sender, ListViewItemClickEventArgs args)
{
    PasteSelectedClip();
}

void QuickPasteWindow::CloseButton_Click(Windows::Foundation::IInspectable sender, Windows::UI::Xaml::RoutedEventArgs args)
{
    HideWindow();
}

void QuickPasteWindow::ShowWindow()
{
    auto window = Window::Current();
    if (window)
    {
        window.Activate();
    }
}

void QuickPasteWindow::HideWindow()
{
    auto window = Window::Current();
    if (window)
    {
        window.Close();
    }
}

void QuickPasteWindow::SetSearchFocus()
{
    SearchBox().Focus(Windows::UI::Xaml::FocusState::Programmatic);
}

void QuickPasteWindow::LoadClips()
{
}

void QuickPasteWindow::FilterClips(winrt::hstring searchText)
{
}

void QuickPasteWindow::PasteSelectedClip()
{
}
