#pragma once
#include "ViewModelBase.h"
#include <winrt/Windows.Foundation.Collections.h>
#include "Models/ClipItem.h"

namespace winrt::DittoWinUI::ViewModels
{
    struct ClipItemViewModel : ViewModelBase
    {
    public:
        ClipItemViewModel(Models::ClipItem item);

        int64_t Id() const { return m_item.Id; }
        winrt::hstring Description() const { return m_item.Description; }
        void SetDescription(winrt::hstring value);
        bool IsPinned() const { return m_item.IsPinned; }
        void SetIsPinned(bool value);
        int32_t GroupId() const { return m_item.GroupId; }
        void SetGroupId(int32_t value);
        Windows::Foundation::DateTime Created() const { return m_item.Created; }
        Windows::Foundation::DateTime Modified() const { return m_item.Modified; }

        Models::ClipItem GetClipItem() { return m_item; }

    private:
        Models::ClipItem m_item;
    };

    struct QuickPasteViewModel : ViewModelBase
    {
    public:
        QuickPasteViewModel();

        winrt::Windows::Foundation::Collections::IVector<ClipItemViewModel> Items();
        winrt::hstring SearchText() const { return m_searchText; }
        void SetSearchText(winrt::hstring value);
        ClipItemViewModel SelectedItem();
        void SetSelectedItem(ClipItemViewModel value);
        int32_t SelectedIndex() const { return m_selectedIndex; }
        void SetSelectedIndex(int32_t value);

        bool IsLoading() const { return m_isLoading; }
        void SetIsLoading(bool value);

        winrt::Windows::UI::Xaml::Input::ICommand PasteSelectedCommand();
        winrt::Windows::UI::Xaml::Input::ICommand DeleteSelectedCommand();
        winrt::Windows::UI::Xaml::Input::ICommand PinSelectedCommand();
        winrt::Windows::UI::Xaml::Input::ICommand EditSelectedCommand();
        winrt::Windows::UI::Xaml::Input::ICommand RefreshCommand();
        winrt::Windows::UI::Xaml::Input::ICommand MoveToGroupCommand();

        Windows::Foundation::IAsyncOperation<void> LoadItemsAsync();
        Windows::Foundation::IAsyncOperation<void> PasteSelectedAsync();
        void DeleteSelected();
        void PinSelected();
        void EditSelected();
        void Refresh();
        void MoveToGroup(int32_t groupId);

    private:
        void FilterItems();
        winrt::hstring m_searchText;
        int32_t m_selectedIndex{ -1 };
        bool m_isLoading{ false };
        winrt::Windows::Foundation::Collections::IVector<ClipItemViewModel> m_items;
        winrt::Windows::Foundation::Collections::IVector<ClipItemViewModel> m_filteredItems;
        ClipItemViewModel m_selectedItem{ nullptr };
        winrt::com_ptr<RelayCommand> m_pasteSelectedCommand;
        winrt::com_ptr<RelayCommand> m_deleteSelectedCommand;
        winrt::com_ptr<RelayCommand> m_pinSelectedCommand;
        winrt::com_ptr<RelayCommand> m_editSelectedCommand;
        winrt::com_ptr<RelayCommand> m_refreshCommand;
        winrt::com_ptr<RelayCommand> m_moveToGroupCommand;
    };
}
