#pragma once
#include "../llri.h"

#ifdef MERCURY_GRAPHICS_API_D3D12
#include "mercury_log.h"
#include <system_error>
#include "../platform.h"

#define RPC_NO_WINDOWS_H
#define NOMINMAX

#include <directx/d3d12.h>
#include "directx/d3dx12.h"
#include <dxgi1_4.h>

extern IDXGIFactory4* gFactory;
extern ID3D12Debug1* gDebugController;
extern IDXGIAdapter1* gAdapter;
extern ID3D12Device* gDevice;
extern ID3D12DebugDevice* gDebugDevice;
extern ID3D12CommandQueue* gCommandQueue;
extern ID3D12CommandAllocator* gCommandAllocator;
extern ID3D12DescriptorHeap* gDescriptorsHeapRTV;
extern ID3D12CommandList* gCurrentCommandBuffer;

#define D3D_CALL(func) {HRESULT res = (func); if(res < 0){ mercury::platform::fatalFail( std::system_category().message(res).c_str() ); } }

#endif