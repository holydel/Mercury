#include "mercury_api.h"

#include "mercury_imgui.h"


#include "imgui.h"


#ifdef MERCURY_PLATFORM_WINDOWS
#include "backends/imgui_impl_win32.cpp"
#endif

#ifdef MERCURY_PLATFORM_ANDROID
#include "backends/imgui_impl_android.cpp"
#endif

#ifdef MERCURY_PLATFORM_LINUX
#include "backends/imgui_impl_android.cpp"
#endif

#ifdef MERCURY_PLATFORM_MACOS
#include "backends/imgui_impl_android.cpp"
#endif

#ifdef MERCURY_GRAPHICS_API_VULKAN
#include "llri_vulkan/mercury_vulkan.h"
#include "backends/imgui_impl_vulkan.cpp"
#endif


void mercury::imgui::initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	


}

void mercury::imgui::shutdown()
{

}
