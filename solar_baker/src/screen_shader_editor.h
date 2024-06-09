#pragma once

#include "screen_shader_editor.g.h"

namespace winrt::SolarBaker::implementation
{
    struct screen_shader_editor : screen_shader_editorT<screen_shader_editor>
    {
        screen_shader_editor() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::SolarBaker::factory_implementation
{
    struct screen_shader_editor : screen_shader_editorT<screen_shader_editor, implementation::screen_shader_editor>
    {
    };
}
