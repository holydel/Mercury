#pragma once
#include "mercury_imgui_interface.h"

class TextEditor;

class ShadersEditorScreen : public mercury::imgui_interface::Screen
{
	TextEditor *textEditor = nullptr;
public:
	ShadersEditorScreen();
	~ShadersEditorScreen();
	// Inherited via Screen
	void Draw() override;
};