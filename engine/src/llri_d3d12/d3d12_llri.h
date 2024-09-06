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
#include <D3D12MemAlloc.h>

extern IDXGIFactory4* gFactory;
extern ID3D12Debug1* gDebugController;
extern IDXGIAdapter1* gAdapter;
extern ID3D12Device* gDevice;
extern ID3D12DebugDevice* gDebugDevice;
extern ID3D12CommandQueue* gCommandQueue;
extern ID3D12CommandAllocator* gCommandAllocator;
extern ID3D12DescriptorHeap* gDescriptorsHeapRTV;
extern ID3D12DescriptorHeap* gDescriptorsHeapDSV;
extern ID3D12GraphicsCommandList* gCurrentCommandBuffer;

extern std::vector<CD3DX12_SHADER_BYTECODE> gAllShaders;
extern std::vector<ID3D12RootSignature*> gAllSignatures;
extern std::vector<ID3D12PipelineState*> gAllPSOs;

extern D3D12MA::Allocator* gAllocator;

extern std::vector<ID3D12Resource*> gAllBuffers;

struct BufferMeta
{
	void* mappedPointer;
	D3D12MA::Allocation* allocation;
	D3D12_GPU_VIRTUAL_ADDRESS bufferGPULocation;
	UINT sizeInBytes;
	//VmaAllocation allocation;
	//VmaAllocationInfo allocInfo;
	//VkDeviceMemory dedicatedMemory;
	//VkDeviceAddress deviceAddress;
};

extern std::vector<BufferMeta> gAllBuffersMeta;

struct PSOMeta
{
	D3D12_PRIMITIVE_TOPOLOGY topology;
	mercury::u16 stride;
};

extern std::vector<PSOMeta> gAllPSOMetas;

#define D3D_CALL(func) {HRESULT res = (func); if(res < 0){ mercury::platform::fatalFail( std::system_category().message(res).c_str() ); } }

#endif