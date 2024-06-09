#include "screen_main.h"
#include "imgui.h"


void MainScreen::Draw()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New..."))
		{
		}

		if (ImGui::MenuItem(u8"Open...")) {}

		if (ImGui::BeginMenu("Open Recent", false))
		{
			ImGui::EndMenu();
		}

		bool selected = false;
		if (ImGui::MenuItem("Save", "Ctrl+S", &selected))
		{
		}

		if (ImGui::MenuItem("Save As...")) {}

		ImGui::Separator();

		if (ImGui::MenuItem("Exit", "Alt+F4")) {
			extern bool gIsRunning;
			gIsRunning = false;
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
		if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
		ImGui::Separator();
		if (ImGui::MenuItem("Cut", "CTRL+X")) {}
		if (ImGui::MenuItem("Copy", "CTRL+C")) {}
		if (ImGui::MenuItem("Paste", "CTRL+V")) {}
		
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Assets"))
	{
		if (ImGui::MenuItem("Shaders")) {
			mercury::imgui_interface::AddScreen(&shadersScreen);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}
