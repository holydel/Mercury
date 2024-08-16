#include "screen_shaders_editor.h"
#include "imgui.h"
#include "../../third_party/ImGuiColorTextEdit/TextEditor.h"
#include "shader_compiler.h"
#include "sbproj_source.h"
#include <fstream>

ShadersEditorScreen::ShadersEditorScreen()
{
	textEditor = new TextEditor();
	
	textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());

	textEditor->SetText("//we will be using glsl version 4.5 syntax\n");

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
				auto temp = ShaderCompiler::CompileShader(currentSource);
				int a = 52;
			}
			bool is_selected = false;
			if (ImGui::MenuItem(u8"Save","CTRL+S",&is_selected, currentSource != nullptr))
			{
				currentSource->cachedSource = textEditor->GetText();
				std::ofstream file(currentSource->fullPath);
				file << currentSource->cachedSource;
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

void ShadersEditorScreen::SetShaderSource(SBProjShaderSource* src)
{
	currentSource = src;

	std::ifstream file(src->fullPath);
	if (file)
	{
		file.seekg(0, std::ios::end);
		std::streampos          length = file.tellg();
		file.seekg(0, std::ios::beg);

		src->cachedSource.resize(length);

		file.read(src->cachedSource.data(), length);

		textEditor->SetText(src->cachedSource.c_str());

		int a = 42;
	}
}