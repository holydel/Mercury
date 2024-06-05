//
// Created by holyd on 05.06.2024.
//

#pragma once

#include "imgui.h"      // IMGUI_IMPL_API
#include "emscripten/html5_webgpu.h"

IMGUI_IMPL_API bool     ImGui_ImplEmscripten_Init();
IMGUI_IMPL_API void     ImGui_ImplEmscripten_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplEmscripten_NewFrame();
IMGUI_IMPL_API bool     ImGui_ImplEmscripten_Event();
