#include "pch.h"
#include "mercury_imgui_interface.h"
#if __has_include("mercury_imgui_interface.g.cpp")
#include "mercury_imgui_interface.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Mercury::implementation
{
    int32_t mercury_imgui_interface::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void mercury_imgui_interface::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void mercury_imgui_interface::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
