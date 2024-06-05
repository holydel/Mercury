//
// Created by holyd on 05.06.2024.
//
#include "imgui_impl_emscripten.h"
#ifdef MERCURY_PLATFORM_EMSCRIPTEN
IMGUI_IMPL_API bool     ImGui_ImplEmscripten_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_emscripten";

    io.DisplaySize = ImVec2(1280,720);

    return true;
}

IMGUI_IMPL_API void     ImGui_ImplEmscripten_Shutdown()
{

}

IMGUI_IMPL_API void     ImGui_ImplEmscripten_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup time step
    io.DeltaTime = 1.0f / 60.0f;
}

IMGUI_IMPL_API bool     ImGui_ImplEmscripten_Event()
{
    ImGuiIO& io = ImGui::GetIO();

    extern float emMouseX;
    extern float emMouseY;
    extern bool emMouseDown[5];

    io.MousePos = ImVec2(emMouseX, emMouseY);
    for(int i=0;i<5;++i)
        io.MouseDown[i] = emMouseDown[i];

    return true;
}
#endif