#include "d3d12_llri.h"
#ifdef MERCURY_GRAPHICS_API_D3D12
#include "../application.h"
#include "../platform.h"
#include "d3d12_utils.h"

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
ID3D12DescriptorHeap* gDescriptorsHeapDSV = nullptr;
ID3D12GraphicsCommandList* gCurrentCommandBuffer = nullptr;

std::vector<CD3DX12_SHADER_BYTECODE> gAllShaders;
std::vector<ID3D12RootSignature*> gAllSignatures;
std::vector<ID3D12PipelineState*> gAllPSOs;
D3D12MA::Allocator* gAllocator = nullptr;

std::vector<ID3D12Resource*> gAllBuffers;
std::vector<BufferMeta> gAllBuffersMeta;
std::vector<PSOMeta> gAllPSOMetas;

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
	AllocConsole();

	mercury::write_log_message("LowLevel Rendering Interface Initialize: D3D12");

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

	D3D_CALL(gDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&gCommandAllocator)));


	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 64;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		D3D_CALL(gDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&gDescriptorsHeapRTV)));
		gDescriptorsHeapRTV->SetName(L"RTV Heap");
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 64;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		D3D_CALL(gDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&gDescriptorsHeapDSV)));
		gDescriptorsHeapDSV->SetName(L"DSV Heap");
	}

	D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
	allocatorDesc.pDevice = gDevice;
	allocatorDesc.pAdapter = gAdapter;
	// These flags are optional but recommended.
	allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED |
		D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;

	HRESULT hr = D3D12MA::CreateAllocator(&allocatorDesc, &gAllocator);

	return true;
}

void llri::shutdown()
{
	mercury::write_log_message("LowLevel Rendering Interface Shutdown: D3D12");
}

bool llri::update()
{
	return true;
}



mercury::Shader llri::create_shader_from_bytecode(mercury::Shader::ByteCode bc)
{
	u32 curShaderID = gAllShaders.size();

	gAllShaders.push_back(CD3DX12_SHADER_BYTECODE(bc.data,bc.size));

	return mercury::Shader{curShaderID};
}



mercury::Material llri::create_material(mercury::Material::Desc desc)
{
	mercury::Material result;

	D3D12_ROOT_PARAMETER rootParameters[32] = {};
	int numRootParameters = 0;

	if (desc.shaderInputs.numPushConstants > 0)
	{
		auto& p = rootParameters[numRootParameters];

		p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		p.Constants.Num32BitValues = desc.shaderInputs.numPushConstants;
		p.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		p.Constants.RegisterSpace = 0;
		p.Constants.ShaderRegister = 0;

		numRootParameters++;
	}

	auto IAstate = CD3DX12_INPUT_LAYOUT_SUBOBJECT();
	u16 vertexStride = 0;

	for (auto& vi : desc.vertexInput.inputs)
	{
		D3D12_INPUT_ELEMENT_DESC ie = {};
		ie.AlignedByteOffset = vi.offset;
		ie.Format = d3d12_utils::GetFormatFromMercuryFormat(vi.fmt);
		ie.InstanceDataStepRate = 0;
		ie.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		ie.SemanticIndex = 0;// vi.location;
		ie.InputSlot = vi.bindingPoint;
		ie.SemanticName = vi.semanticName;

		vertexStride += mercury::utils::GetFormatSizeBits(vi.fmt) / 8;
		IAstate.AddInputLayoutElementDesc(ie);
	}

	IAstate.Finalize();

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	if (!desc.vertexInput.inputs.empty())
	{
		rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}
	

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(numRootParameters, rootParameters, 0, nullptr, rootSignatureFlags);

	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;
	D3D_CALL(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	
	u32 curSignatureID = gAllSignatures.size();
	gAllSignatures.push_back(nullptr);
	gAllPSOs.push_back(nullptr);
	auto& meta = gAllPSOMetas.emplace_back();

	D3D_CALL(gDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&gAllSignatures[curSignatureID])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = IAstate;
	
	psoDesc.pRootSignature = gAllSignatures[curSignatureID];
	psoDesc.VS = gAllShaders[desc.vertexShader.handle];
	psoDesc.PS = gAllShaders[desc.fragmentShader.handle];
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise = true;

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	D3D_CALL(gDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&gAllPSOs[curSignatureID])));

	meta.topology = d3d12_utils::GetTopologyFromMercuryTopology(desc.topology);
	meta.stride = vertexStride;
	return mercury::Material{ curSignatureID };
}

mercury::Buffer llri::create_buffer(mercury::u64 size, mercury::Buffer::HeapType heapType, mercury::Buffer::BufferType btype)
{
	ID3D12Resource* buffer = nullptr;
	auto& meta = gAllBuffersMeta.emplace_back();

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = size;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12MA::ALLOCATION_DESC allocationDesc = {};

	switch (heapType)
	{
	case mercury::Buffer::HeapType::DEFAULT:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		break;
	case mercury::Buffer::HeapType::UPLOAD:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		break;
	case mercury::Buffer::HeapType::READBACK:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
		break;
	default:
		break;
	}

	D3D12_RESOURCE_STATES bufferState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

	HRESULT hr = gAllocator->CreateResource(
		&allocationDesc,
		&resourceDesc,
		bufferState,
		NULL,
		&meta.allocation,
		IID_NULL, NULL);
	
	buffer = meta.allocation->GetResource();
	gAllBuffers.emplace_back(buffer);
	D3D_CALL(buffer->Map(0, nullptr, &meta.mappedPointer));

	meta.bufferGPULocation = buffer->GetGPUVirtualAddress();
	meta.sizeInBytes = size;

	return mercury::Buffer{ (u32)gAllBuffers.size() - 1 };
}

void* llri::buffer_mapped_data(mercury::u32 handle)
{
	return gAllBuffersMeta[handle].mappedPointer;
}
#endif
