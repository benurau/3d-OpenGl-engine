#pragma once

#include "wavparse.g.h"

namespace winrt::killdeathcpp::implementation
{
    struct wavparse : wavparseT<wavparse>
    {
        wavparse();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::killdeathcpp::factory_implementation
{
    struct wavparse : wavparseT<wavparse, implementation::wavparse>
    {
    };
}
