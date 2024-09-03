#include "screen_shaders_editor.h"
#include "imgui.h"
#include "../../third_party/ImGuiColorTextEdit/TextEditor.h"
#include "shader_compiler.h"
#include "sbproj_source.h"
#include <fstream>
#include "material_preview.h"

void ShadersEditorScreen::CompileShader()
{
	currentSource->cachedSource = textEditor->GetText();
	currentSource->stage = currentStage;
	currentSource->entryPoint = entryPointName;

	auto temp = ShaderCompiler::CompileShader(currentSource, errors);
	currentErrorIndex = -1;

	if (!currentSource->cachedSPIRV.empty())
	{
		MaterialPreview::Instance().UpdateShader(currentSource->cachedSPIRV, currentSource->stage);
	}
}

ShadersEditorScreen::ShadersEditorScreen()
{
	textEditor = new TextEditor();
	
	textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());

	textEditor->SetText("//we will be using glsl version 4.5 syntax\n");	
}

ShadersEditorScreen::~ShadersEditorScreen()
{
}

std::string trim(const std::string& str) {
	size_t start = 0;
	size_t end = str.length();

	// Trim leading whitespace
	while (start < end && std::isspace(str[start])) {
		start++;
	}

	// Trim trailing whitespace
	while (end > start && std::isspace(str[end - 1])) {
		end--;
	}

	return str.substr(start, end - start);
}

void ShadersEditorScreen::Draw()
{
	std::string winName = "Shaders";

	if (currentSource)
	{
		if (textEditor->IsTextChanged())
		{
			
			if (!isSrcChanged)
			{
				std::string s1 = textEditor->GetText();
				std::string s2 = currentSource->cachedSource;
				s1 = trim(s1);
				s2 = trim(s2);
				int v = strcmp(s1.c_str(), s2.c_str());

				if (v)
				{
					isSrcChanged = true;
				}				
			}			
		}

		winName = currentSource->name;

		if (isSrcChanged)
		{
			winName += "*";
		}
	}

	winName += "###Shaders";

	if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S))
	{
		SaveShader();
	}

	if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Q))
	{
		CompileShader();
	}

	if (isNeedFocus)
	{
		isNeedFocus = false;
		ImGui::SetNextWindowFocus();
	}

	if (ImGui::Begin(winName.c_str(), &is_active, ImGuiWindowFlags_MenuBar))
	{
		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu(u8"Shader"))
		{
			if (ImGui::MenuItem(u8"Compile", "CTRL+Q"))
				CompileShader();

			bool is_selected = false;
			if (ImGui::MenuItem(u8"Save","CTRL+S",&is_selected, currentSource != nullptr))
			{
				SaveShader();
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



		if (ImGui::Button("Compile"))
			CompileShader();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);
		ImGui::Combo("Stage: ", (int*)&currentStage, mercury::Shader::GetHumanReadableShaderNames(), (int)mercury::Shader::Stage::MAX);
		
		ImGui::SameLine();
		ImGui::SetNextItemWidth(260);
		ImGui::InputText("Entry Point: ", entryPointName, sizeof(entryPointName));
		
		ImVec2 srcRegion = ImGui::GetContentRegionAvail();

		int errorsHeight = errors.size() * (ImGui::GetTextLineHeight() + 4) + (errors.size() > 0 ? 12 : 0);
		srcRegion.y -= errorsHeight;

		textEditor->Render("Source", srcRegion,true);

		//render errors
		//ImGui::ListBox("errors")
		if (errors.size() > 0)
		{
			std::vector<const char*> errors_labels(errors.size());
			for (int i = 0; i < errors.size(); ++i)
			{
				errors_labels[i] = errors[i].error.c_str();
			}

			currentErrorIndex = -1;
			ImGui::ListBox("errors", &currentErrorIndex, errors_labels.data(), errors.size());

			if (currentErrorIndex != -1)
			{
				auto err = errors[currentErrorIndex];
				TextEditor::Coordinates start;
				TextEditor::Coordinates end;

				start.mColumn = 0;
				start.mLine = err.line;

				end.mColumn = 1;
				end.mLine = err.line;

				textEditor->SetSelection(start, end, TextEditor::SelectionMode::Line);
			}
		}

		ImGui::End();
	}
}

void ShadersEditorScreen::SaveShader()
{
	currentSource->cachedSource = textEditor->GetText();
	std::ofstream file(currentSource->fullPath);
	file << currentSource->cachedSource;
	isSrcChanged = false;
}

void ShadersEditorScreen::SetShaderSource(SBProjShaderSource* src)
{
	currentSource = src;

	std::ifstream file(src->fullPath);

	std::string ext = src->fullPath.extension().generic_u8string();
	for (char& c : ext)
		c = std::tolower(c);

	if (ext == ".hlsl")
	{
		textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
	}
	else
	{
		currentStage = src->stage;
		textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
	}

	if (file)
	{
		//std::string file_content;
		std::getline(file, src->cachedSource, '\0');

		//file.seekg(0, std::ios::end);
		//std::streampos          length = file.tellg();
		//file.seekg(0, std::ios::beg);

		//src->cachedSource.resize(length,'x');

		//file.read(src->cachedSource.data(), length);

		textEditor->SetText(src->cachedSource);
		errors.clear();
		currentErrorIndex = -1;
		isSrcChanged = false;
		isNeedFocus = true;
		int a = 42;
	}

	strcpy_s(entryPointName, "main");
}