#include "mercury_api.h"
#ifdef MERCURY_PLATFORM_MACOS
#define IMGUI_IMPL_METAL_CPP_EXTENSIONS
#include "backends/imgui_impl_osx.mm"
#endif