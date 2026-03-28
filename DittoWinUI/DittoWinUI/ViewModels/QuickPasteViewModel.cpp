#include "QuickPasteViewModel.h"
#include <algorithm>

using namespace winrt::DittoWinUI::ViewModels;
using namespace winrt::DittoWinUI::Services;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Input;

ClipItemViewModel::ClipItemViewModel(Models::ClipItem item)
    : m_item(item)
{
}

void ClipItemViewModel::SetDescription(winrt::hstring value)
{
    if (m_item.Description != value)
    {
        m_item.Description = value;
        OnPropertyChanged(L"Description");
    }
}

void ClipItemViewModel::SetIsPinned(bool value)
{
    if (m_item.IsPinned != value)
    {
        m_item.IsPinned = value;
        OnPropertyChanged(L"IsPinned");
    }
}

void ClipItemViewModel::SetGroupId(int32_t value)
{
    if (m_item.GroupId != value)
    {
        m_item.GroupId = value;
        OnPropertyChanged(L"GroupId");
    }
}

QuickPasteViewModel::QuickPasteViewModel()
    : m_items(single_threaded_vector<ClipItemViewModel>())
    , m_filteredItems(single_threaded_vector<ClipItemViewModel>())
{
    m_pasteSelectedCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        PasteSelectedAsync().get();
    });

    m_deleteSelectedCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        DeleteSelected();
    });

    m_pinSelectedCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        PinSelected();
    });

    m_editSelectedCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        EditSelected();
    });

    m_refreshCommand = make<RelayCommand>([this](ICommand, IInspectable) {
        Refresh();
    });

    m_moveToGroupCommand = make<RelayCommand>([this](ICommand, IInspectable) {
    });
}

void QuickPasteViewModel::SetSearchText(winrt::hstring value)
{
    if (m_searchText != value)
    {
        m_searchText = value;
        OnPropertyChanged(L"SearchText");
        FilterItems();
    }
}

void QuickPasteViewModel::SetSelectedIndex(int32_t value)
{
    if (m_selectedIndex != value)
    {
        m_selectedIndex = value;
        OnPropertyChanged(L"SelectedIndex");
    }
}

void QuickPasteViewModel::SetSelectedItem(ClipItemViewModel value)
{
    m_selectedItem = value;
    OnPropertyChanged(L"SelectedItem");
}

void QuickPasteViewModel::SetIsLoading(bool value)
{
    if (m_isLoading != value)
    {
        m_isLoading = value;
        OnPropertyChanged(L"IsLoading");
    }
}

void QuickPasteViewModel::FilterItems()
{
    m_filteredItems.Clear();

    if (m_searchText.empty())
    {
        for (const auto& item : m_items)
        {
            m_filteredItems.Append(item);
        }
    }
    else
    {
        std::wstring searchLower = m_searchText.c_str();
        std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), towlower);

        for (const auto& item : m_items)
        {
            std::wstring desc = item.Description().c_str();
            std::transform(desc.begin(), desc.end(), desc.begin(), towlower);

            if (desc.find(searchLower) != std::wstring::npos)
            {
                m_filteredItems.Append(item);
            }
        }
    }

    OnPropertyChanged(L"FilteredItems");
}

IAsyncOperation<void> QuickPasteViewModel::LoadItemsAsync()
{
    SetIsLoading(true);

    co_await resume_background();

    auto& db = ServiceLocator::Instance().Database();
    auto filter = Models::QueryFilter::Default();
    auto clips = co_await db.QueryClipsAsync(filter);

    m_items.Clear();
    for (const auto& clip : clips)
    {
        m_items.Append(make<ClipItemViewModel>(clip));
    }

    FilterItems();
    SetIsLoading(false);

    co_return;
}

IAsyncOperation<void> QuickPasteViewModel::PasteSelectedAsync()
{
    co_await resume_background();

    if (m_selectedItem)
    {
        auto& clipboard = ServiceLocator::Instance().Clipboard();
        co_await clipboard.SetClipboardContentAsync(m_selectedItem.GetClipItem());
    }

    co_return;
}

void QuickPasteViewModel::DeleteSelected()
{
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int32_t>(m_items.Size()))
    {
        m_items.RemoveAt(static_cast<uint32_t>(m_selectedIndex));
        FilterItems();
    }
}

void QuickPasteViewModel::PinSelected()
{
    if (m_selectedItem)
    {
        m_selectedItem.SetIsPinned(!m_selectedItem.IsPinned());
    }
}

void QuickPasteViewModel::EditSelected()
{
}

void QuickPasteViewModel::Refresh()
{
    LoadItemsAsync();
}

void QuickPasteViewModel::MoveToGroup(int32_t groupId)
{
    if (m_selectedItem)
    {
        m_selectedItem.SetGroupId(groupId);
    }
}

ICommand QuickPasteViewModel::PasteSelectedCommand()
{
    return *m_pasteSelectedCommand;
}

ICommand QuickPasteViewModel::DeleteSelectedCommand()
{
    return *m_deleteSelectedCommand;
}

ICommand QuickPasteViewModel::PinSelectedCommand()
{
    return *m_pinSelectedCommand;
}

ICommand QuickPasteViewModel::EditSelectedCommand()
{
    return *m_editSelectedCommand;
}

ICommand QuickPasteViewModel::RefreshCommand()
{
    return *m_refreshCommand;
}

ICommand QuickPasteViewModel::MoveToGroupCommand()
{
    return *m_moveToGroupCommand;
}
