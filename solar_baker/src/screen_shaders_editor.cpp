#include "screen_shaders_editor.h"
#include "imgui.h"
#include "../../third_party/ImGuiColorTextEdit/TextEditor.h"

ShadersEditorScreen::ShadersEditorScreen()
{
	textEditor = new TextEditor();
	
	textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

ShadersEditorScreen::~ShadersEditorScreen()
{
}

void ShadersEditorScreen::Draw()
{
	if (ImGui::Begin(u8"Shaders", &is_active, ImGuiWindowFlags_MenuBar))
	{
		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu(u8"Shader"))
		{
			if (ImGui::MenuItem(u8"Compile"))
			{
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Get IL", true))
			{
				if (ImGui::MenuItem(u8"SPIR-V")) {}
				if (ImGui::MenuItem(u8"DXIL")) {}
				if (ImGui::MenuItem(u8"MSL")) {}

				ImGui::EndMenu();
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


		ImGui::EndMenuBar();

		textEditor->Render("Source");
		ImGui::End();
	}
}
