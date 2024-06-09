#pragma once
#include "mercury_imgui_interface.h"

class ShadersEditorScreen : public mercury::imgui_interface::Screen
{
public:

	// Inherited via Screen
	void Draw() override;
};