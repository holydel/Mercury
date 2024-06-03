#pragma once
#include "d3d12_llri.h"

#ifdef MERCURY_GRAPHICS_API_D3D12
#include "../swapchain.h"
extern IDXGISwapChain1* gSwapChain;
extern DXGI_FORMAT gSwapChainFormat;
#endif