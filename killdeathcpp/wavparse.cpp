#include "pch.h"
#include "wavparse.h"
#if __has_include("wavparse.g.cpp")
#include "wavparse.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::killdeathcpp::implementation
{
    wavparse::wavparse()
    {
        InitializeComponent();
    }

    int32_t wavparse::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void wavparse::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void wavparse::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
