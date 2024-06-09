#include "pch.h"
#include "screen_shader_editor.h"
#if __has_include("screen_shader_editor.g.cpp")
#include "screen_shader_editor.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::SolarBaker::implementation
{
    int32_t screen_shader_editor::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void screen_shader_editor::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void screen_shader_editor::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
