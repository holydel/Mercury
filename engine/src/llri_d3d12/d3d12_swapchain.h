#pragma once
#include "d3d12_llri.h"

#ifdef MERCURY_GRAPHICS_API_D3D12
#include "../swapchain.h"
extern IDXGISwapChain3* gSwapChain;
extern DXGI_FORMAT gSwapChainFormat;

extern ID3D12Resource* gIntermediateMSAATarget;
extern ID3D12Resource* gIntermediateDSBuffer; //can be multisampled or not
#endif