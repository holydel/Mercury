#include "d3d12_llri.h"
#ifdef MERCURY_GRAPHICS_API_D3D12
#include "../application.h"
#include "../platform.h"


#include "../swapchain.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

IDXGIFactory4* gFactory = nullptr;
ID3D12Debug1* gDebugController = nullptr;
IDXGIAdapter1* gAdapter = nullptr;
ID3D12Device* gDevice = nullptr;
ID3D12DebugDevice* gDebugDevice = nullptr;
ID3D12CommandQueue* gCommandQueue = nullptr;
ID3D12CommandAllocator* gCommandAllocator = nullptr;
ID3D12DescriptorHeap* gDescriptorsHeapRTV = nullptr;

using namespace mercury;

i8 ChoosePhysicalDeviceByHeuristics(std::vector<DXGI_ADAPTER_DESC1>& adescs)
{
	return 0;
}

void ChoosePhysicalDevice()
{
	const auto& renderCfg = gApplication->config.render;

	IDXGIAdapter1* adapter = nullptr;
	std::vector<DXGI_ADAPTER_DESC1> allAdapterDescs;
	allAdapterDescs.reserve(8);

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != gFactory->EnumAdapters1(i, &adapter); ++i)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		allAdapterDescs.push_back(desc);

		write_log_message(L"Found D3D12 device (%d): %s", i, desc.Description);
		adapter->Release();
	}

	auto selectedAdapterID = renderCfg.adapterID == -1 ? ChoosePhysicalDeviceByHeuristics(allAdapterDescs) : renderCfg.adapterID;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != gFactory->EnumAdapters1(i, &adapter); ++i)
	{
		if (i == selectedAdapterID)
		{
			gAdapter = adapter;
			auto& desc = allAdapterDescs[i];

			write_log_message(L"Selected D3D12 device (%d): %s", i, desc.Description);
			break;
		}
		else
		{
			adapter->Release();
		}
	}
}

bool llri::initialize()
{
	const auto& renderCfg = gApplication->config.render;

	UINT dxgiFactoryFlags = 0;

	if (renderCfg.enableDebugLayers)
	{
		ID3D12Debug* dc;
		D3D_CALL(D3D12GetDebugInterface(IID_PPV_ARGS(&dc)));
		D3D_CALL(dc->QueryInterface(IID_PPV_ARGS(&gDebugController)));
		gDebugController->EnableDebugLayer();
		gDebugController->SetEnableGPUBasedValidation(true);

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

		dc->Release();
		dc = nullptr;
	}

	D3D_CALL(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&gFactory)));

	ChoosePhysicalDevice();

	D3D_CALL(D3D12CreateDevice(gAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&gDevice)));

	if (renderCfg.enableDebugLayers)
	{
		D3D_CALL(gDevice->QueryInterface(&gDebugDevice));
	}


	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D_CALL(gDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&gCommandQueue)));

	D3D_CALL(gDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,	IID_PPV_ARGS(&gCommandAllocator)));


	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 64;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//desc.Flags = 0;// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	D3D_CALL(gDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&gDescriptorsHeapRTV)));

	return true;
}

void llri::shutdown()
{

}

bool llri::update()
{

	return true;}

llri::CommandList llri::BeginOneTimeSubmitCommandList()
{
	void* cmdListPtr = nullptr;

	llri::CommandList result = { cmdListPtr };
	return result;
}

void llri::EndOneTimeSubmitCommandList(llri::CommandList clist)
{

}
#endif
