#include "d3d12_swapchain.h"
#include <chrono>
#include "../mercury_imgui.h"
#include "../engine.h"

using namespace mercury;

#ifdef MERCURY_GRAPHICS_API_D3D12
struct FrameObject
{
	HANDLE fenceEvent;
	ID3D12Fence* fence;
	UINT64 fenceValue;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;

	CD3DX12_CPU_DESCRIPTOR_HANDLE bbRTV;
	ID3D12Resource* bbResource;
};

int gNumFrames = 3;
std::vector<FrameObject> gFrames;
IDXGISwapChain3* gSwapChain = nullptr;
u64 gCurrentFrameIndex = 0;
u64 gFrameID = 0;
DXGI_FORMAT gSwapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
u32 gNewWidth = 0;
u32 gCurWidth = 0;
u32 gNewHeight = 0;
u32 gCurHeight = 0;
u32 gDeltaFrameInBB = 0;

ID3D12Resource* gIntermediateMSAATarget = nullptr;
ID3D12Resource* gIntermediateDSBuffer = nullptr; //can be multisampled or not

bool llri::swapchain::create(void* nativeWindowHandle)
{
	HWND hwnd = static_cast<HWND>(nativeWindowHandle);

	RECT clientRect = {};
	GetClientRect(hwnd, (LPRECT)&clientRect);
	
	DXGI_SWAP_CHAIN_DESC1 sdesc = {};
	sdesc.Width = gNewWidth = gCurWidth = clientRect.right - clientRect.left;
	sdesc.Height = gNewHeight = gCurHeight = clientRect.bottom - clientRect.top;
	sdesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	sdesc.BufferCount = gNumFrames;
	sdesc.Format = gSwapChainFormat;
	sdesc.SampleDesc.Count = 1;
	sdesc.SampleDesc.Quality = 0;
	sdesc.Scaling = DXGI_SCALING_NONE; 
	sdesc.Stereo = false;
	sdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfdesc = {};
	sfdesc.Windowed = true;
	sfdesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	sfdesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sfdesc.RefreshRate.Numerator = 1;
	sfdesc.RefreshRate.Denominator = 60;
	IDXGIOutput* output = nullptr;

	IDXGISwapChain1* swapchain1 = nullptr;
	auto res = gFactory->CreateSwapChainForHwnd(gCommandQueue, hwnd, &sdesc, nullptr, output, &swapchain1);
	swapchain1->QueryInterface(__uuidof(IDXGISwapChain3), (void**) & gSwapChain);

	gFrames.resize(gNumFrames);

	auto rtvDescriptorSize = gDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(gDescriptorsHeapRTV->GetCPUDescriptorHandleForHeapStart());

	for (uint32_t i = 0; i < gNumFrames; ++i)
	{
		FrameObject& fo = gFrames[i];
		D3D_CALL(gDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fo.fence)));

		gSwapChain->GetBuffer(i, IID_PPV_ARGS(&fo.bbResource));

		gDevice->CreateRenderTargetView(fo.bbResource, nullptr, rtvHandle);

		fo.bbRTV = rtvHandle;
		rtvHandle.Offset(rtvDescriptorSize);

		
		D3D_CALL(gDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&fo.commandAllocator)));

		D3D_CALL(gDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, fo.commandAllocator, nullptr, IID_PPV_ARGS(&fo.commandList)));

		D3D_CALL(fo.commandList->Close());

		fo.fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

		int a = 42;
	}

	

	

	//for (int i = 0; i < g_NumFrames; ++i)
	//{
	//	ComPtr<ID3D12Resource> backBuffer;
	//	ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

	//	device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

	//	g_BackBuffers[i] = backBuffer;

	//	rtvHandle.Offset(rtvDescriptorSize);
	//}
	imgui::initialize();
	return true;
}

bool llri::swapchain::destroy()
{
	imgui::shutdown();
	return true;
}

void llri::swapchain::setBackGroundColor(float r, float g, float b, float a)
{

}
uint64_t Signal(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence,
	uint64_t& fenceValue)
{
	uint64_t fenceValueForSignal = ++fenceValue;
	D3D_CALL(commandQueue->Signal(fence, fenceValueForSignal));

	return fenceValueForSignal;

}

void WaitForFenceValue(ID3D12Fence* fence, uint64_t fenceValue, HANDLE fenceEvent,
	std::chrono::milliseconds duration = std::chrono::milliseconds::max())
{
	if (fence->GetCompletedValue() < fenceValue)
	{
		D3D_CALL(fence->SetEventOnCompletion(fenceValue, fenceEvent));
		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
	}
}

void Flush(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence,
	uint64_t& fenceValue, HANDLE fenceEvent)
{
	uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
	WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
}

void llri::swapchain::resize(mercury::u32 newWidth, mercury::u32 newHeight)
{
	gNewWidth = newWidth;
	gNewHeight = newHeight;
}

extern glm::vec4 gCurrentViewport;

bool llri::swapchain::update()
{
	if (gNewWidth != gCurWidth || gNewHeight != gCurHeight)
	{
		gCurWidth = gNewWidth;
		gCurHeight = gNewHeight;

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			Flush(gCommandQueue, gFrames[i].fence, gFrames[i].fenceValue, gFrames[i].fenceEvent);

			gFrames[i].bbResource->Release();
			gFrames[i].fenceValue = gFrames[gCurrentFrameIndex].fenceValue;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		D3D_CALL(gSwapChain->GetDesc(&swapChainDesc));

		D3D_CALL(gSwapChain->ResizeBuffers(swapChainDesc.BufferCount, gCurWidth, gCurHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));


		auto rtvDescriptorSize = gDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(gDescriptorsHeapRTV->GetCPUDescriptorHandleForHeapStart());

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			FrameObject& fo = gFrames[i];
			gSwapChain->GetBuffer(i, IID_PPV_ARGS(&fo.bbResource));
			gDevice->CreateRenderTargetView(fo.bbResource, nullptr, rtvHandle);
			fo.bbRTV = rtvHandle;
			rtvHandle.Offset(rtvDescriptorSize);
		}

		//u32 curBBFrame = gSwapChain->GetCurrentBackBufferIndex();
		//gDeltaFrameInBB = curBBFrame - (gFrameID % gNumFrames);
	}

	gCurrentFrameIndex = gSwapChain->GetCurrentBackBufferIndex();

	auto& frame = gFrames[gCurrentFrameIndex];
	
	frame.commandAllocator->Reset();
	frame.commandList->Reset(frame.commandAllocator, nullptr);
	gCurrentCommandBuffer = frame.commandList;

	llri::context ctx{ static_cast<void*>(frame.commandList) };
	engine::renderBeforeCallback(ctx);

	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			frame.bbResource,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		frame.commandList->ResourceBarrier(1, &barrier);

		float r = sinf((float)gFrameID * 0.005f) * 0.5f + 0.5f;
		float g = cosf((float)gFrameID * 0.006f) * 0.5f + 0.5f;
		float b = sinf((float)gFrameID * 0.05f) * 0.5f + 0.5f;

		FLOAT clearColor[] = { r, g, b, 1.0f };

		frame.commandList->ClearRenderTargetView(frame.bbRTV, clearColor, 0, nullptr);

		frame.commandList->OMSetRenderTargets(1, &frame.bbRTV, FALSE, NULL);
	}

	CD3DX12_VIEWPORT viewport = {};
	CD3DX12_RECT scissorRect = {};
	viewport.Width = gCurWidth;
	viewport.MaxDepth = 1.0f;

	//viewport.Height = -((int)gCurHeight);
	//viewport.TopLeftY = gCurHeight;

	viewport.Height = gCurHeight;
	viewport.TopLeftY = 0;

	gCurrentViewport.x = 0;
	gCurrentViewport.y = 0;
	gCurrentViewport.z = gCurWidth;
	gCurrentViewport.w = gCurHeight;

	scissorRect.right = gCurWidth;
	scissorRect.bottom = gCurHeight;
	frame.commandList->RSSetViewports(1, &viewport);
	frame.commandList->RSSetScissorRects(1, &scissorRect);

	engine::renderCallback(ctx);
	engine::renderAfterCallback(ctx);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		frame.bbResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	frame.commandList->ResourceBarrier(1, &barrier);
	D3D_CALL(frame.commandList->Close());

	ID3D12CommandList* const commandLists[] = {
		frame.commandList
	};

	gCommandQueue->ExecuteCommandLists(1, commandLists);

	HRESULT resPresent = gSwapChain->Present(0, DXGI_SWAP_EFFECT_FLIP_DISCARD);

	HRESULT res2 = gDevice->GetDeviceRemovedReason();
	auto resHumanReadable = std::system_category().message(res2);

	frame.fenceValue = Signal(gCommandQueue, frame.fence, frame.fenceValue);

	//auto gD3D12CurrentBackBufferIndex = gSwapChain->GetCurrentBackBufferIndex();

	WaitForFenceValue(frame.fence, frame.fenceValue, frame.fenceEvent);

	gFrameID++;
	return true;
}
#endif