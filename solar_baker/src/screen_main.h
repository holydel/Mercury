#pragma once
#include "mercury_imgui_interface.h"
#include "screen_shaders_editor.h"

class MainScreen : public mercury::imgui_interface::Screen
{
	ShadersEditorScreen shadersScreen;
public:

	// Inherited via Screen
	void Draw() override;
};