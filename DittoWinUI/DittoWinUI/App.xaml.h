#pragma once
#include <windows/applicationstrategy/implementablebase.h>
#include <Microsoft.UI.Xaml/XamlTypeInfo.g.h>

namespace winrt::DittoWinUI::implementation
{
    struct App : AppT<App>
    {
        App();
        void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
        
    private:
        using kind = Windows::ApplicationModel::Activation::ActivationKind;
        void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
        void Initialize();
        void SetWindow();
    };
}
