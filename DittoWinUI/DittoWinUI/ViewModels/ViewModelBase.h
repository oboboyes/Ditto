#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>

namespace winrt::DittoWinUI::ViewModels
{
    struct ViewModelBase : winrt::Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void RemovePropertyChanged(event_token token);

        virtual void RaisePropertyChanged(winrt::hstring propertyName);

    protected:
        virtual void OnPropertyChanged(winrt::hstring propertyName);

    private:
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };

    inline event_token ViewModelBase::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    inline void ViewModelBase::RemovePropertyChanged(event_token token)
    {
        m_propertyChanged.remove(token);
    }

    inline void ViewModelBase::RaisePropertyChanged(winrt::hstring propertyName)
    {
        m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }

    inline void ViewModelBase::OnPropertyChanged(winrt::hstring propertyName)
    {
        RaisePropertyChanged(propertyName);
    }

    struct RelayCommand : winrt::Windows::UI::Xaml::Input::ICommand
    {
    public:
        RelayCommand(Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Input::ICommand, Windows::Foundation::IInspectable> execute);
        RelayCommand(Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Input::ICommand, Windows::Foundation::IInspectable> execute, 
                     Windows::Foundation::Func<Windows::Foundation::IInspectable, bool> canExecute);

        bool CanExecute(Windows::Foundation::IInspectable const& parameter);
        void Execute(Windows::Foundation::IInspectable const& parameter);
        event_token CanExecuteChanged(Windows::Foundation::EventHandler<Windows::Foundation::IInspectable> const& handler);
        void RemoveCanExecuteChanged(event_token token);

    private:
        Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Input::ICommand, Windows::Foundation::IInspectable> m_execute;
        Windows::Foundation::Func<Windows::Foundation::IInspectable, bool> m_canExecute;
        event<Windows::Foundation::EventHandler<Windows::Foundation::IInspectable>> m_canExecuteChanged;
    };
}
