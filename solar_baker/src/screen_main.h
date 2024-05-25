#pragma once
#include "mercury_imgui_interface.h"

class MainScreen : public mercury::imgui_interface::Screen
{
public:

	// Inherited via Screen
	void Draw() override;

	bool IsActive() override
	{
		return true;
	}
};