#pragma once
#include "metal_llri.h"

#ifdef MERCURY_GRAPHICS_API_METAL
#include "../swapchain.h"
extern IDXGISwapChain1* gSwapChain;
extern DXGI_FORMAT gSwapChainFormat;
#endif