#pragma once
#include "mercury_imgui_interface.h"
#include "screen_shaders_editor.h"
#include "mercury_application.h"

class MainScreen : public mercury::imgui_interface::Screen
{
	ShadersEditorScreen shadersScreen;

	std::vector<std::string> recentlyProjects;

public:

	// Inherited via Screen
	void Draw() override;

	void InitializeRecentlyProjects(mercury::Application *app);
	void SaveRecentlyProjects(mercury::Application* app);
};