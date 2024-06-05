#pragma once
#include "../llri.h"

#ifdef MERCURY_GRAPHICS_API_WEBGPU
#include "mercury_log.h"
#include <system_error>
#include "../platform.h"
#include "emscripten/html5_webgpu.h"

extern WGPUDevice gDevice;
extern WGPUAdapter gAdapter;
extern WGPUInstance gInstance;
extern WGPUQueue gQueue;
extern WGPUSurface gSurface;
extern WGPUSwapChain  gSwapChain;
extern WGPUTextureFormat gPrefferedBackbufferFormat;

#endif