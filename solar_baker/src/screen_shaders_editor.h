#pragma once
#include "mercury_imgui_interface.h"
#include "sbproj_source.h"
#include "shader_compiler.h"

class TextEditor;

class ShadersEditorScreen : public mercury::imgui_interface::Screen
{
	TextEditor *textEditor = nullptr;
	SBProjShaderSource* currentSource = nullptr;
	void CompileShader();
	char entryPointName[32] = {};
	mercury::Shader::Stage currentStage = mercury::Shader::Stage::Vertex;
	std::vector<ShaderCompiler::ShaderCompilerErrorInfo> errors;
	int currentErrorIndex = -1;
public:
	ShadersEditorScreen();
	~ShadersEditorScreen();
	// Inherited via Screen
	void Draw() override;

	void SetShaderSource(SBProjShaderSource* src);
};