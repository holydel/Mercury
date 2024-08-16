#pragma once
#include "mercury_imgui_interface.h"
#include "sbproj_source.h"

class TextEditor;

class ShadersEditorScreen : public mercury::imgui_interface::Screen
{
	TextEditor *textEditor = nullptr;
	SBProjShaderSource* currentSource = nullptr;

public:
	ShadersEditorScreen();
	~ShadersEditorScreen();
	// Inherited via Screen
	void Draw() override;

	void SetShaderSource(SBProjShaderSource* src);
};