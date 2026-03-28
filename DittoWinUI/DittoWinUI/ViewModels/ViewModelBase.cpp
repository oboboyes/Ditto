#include "ViewModelBase.h"

using namespace winrt::DittoWinUI::ViewModels;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml::Input;

RelayCommand::RelayCommand(TypedEventHandler<ICommand, IInspectable> execute)
    : m_execute(execute)
    , m_canExecute(nullptr)
{
}

RelayCommand::RelayCommand(TypedEventHandler<ICommand, IInspectable> execute, Func<IInspectable, bool> canExecute)
    : m_execute(execute)
    , m_canExecute(canExecute)
{
}

bool RelayCommand::CanExecute(IInspectable const& parameter)
{
    if (m_canExecute)
    {
        return m_canExecute(parameter);
    }
    return true;
}

void RelayCommand::Execute(IInspectable const& parameter)
{
    if (m_execute)
    {
        m_execute(*this, parameter);
    }
}

event_token RelayCommand::CanExecuteChanged(EventHandler<IInspectable> const& handler)
{
    return m_canExecuteChanged.add(handler);
}

void RelayCommand::RemoveCanExecuteChanged(event_token token)
{
    m_canExecuteChanged.remove(token);
}
