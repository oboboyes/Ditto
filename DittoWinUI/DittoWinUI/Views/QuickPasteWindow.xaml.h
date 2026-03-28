#pragma once
#include "QuickPasteWindow.g.h"

namespace winrt::DittoWinUI::Views::implementation
{
    struct QuickPasteWindow : QuickPasteWindowT<QuickPasteWindow>
    {
        QuickPasteWindow();

        void SearchBox_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox sender, 
            Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs args);
        void SearchBox_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox sender, 
            Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs args);
        void ClipListView_SelectionChanged(Windows::UI::Xaml::Controls::ListView sender, 
            Windows::UI::Xaml::Controls::ListViewSelectionChangedEventArgs args);
        void ClipListView_ItemClick(Windows::UI::Xaml::Controls::ListView sender, 
            Windows::UI::Xaml::Controls::ListViewItemClickEventArgs args);
        void CloseButton_Click(Windows::Foundation::IInspectable sender, 
            Windows::UI::Xaml::RoutedEventArgs args);

        void ShowWindow();
        void HideWindow();
        void SetSearchFocus();

    private:
        void LoadClips();
        void FilterClips(winrt::hstring searchText);
        void PasteSelectedClip();
    };
}
