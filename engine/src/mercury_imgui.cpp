#include "mercury_api.h"

#include "mercury_imgui.h"
#include "platform.h"


#include "imgui.h"


#ifdef MERCURY_PLATFORM_WINDOWS
#include "backends/imgui_impl_win32.cpp"
#endif

#ifdef MERCURY_PLATFORM_EMSCRIPTEN
#include "emscripten/imgui_impl_emscripten.h"
#endif

#ifdef MERCURY_PLATFORM_ANDROID
#include "backends/imgui_impl_android.cpp"
#endif

#ifdef MERCURY_PLATFORM_LINUX
#include "third_party/imgui_impl_x11.h"
#endif

#ifdef MERCURY_PLATFORM_MACOS
#define IMGUI_IMPL_METAL_CPP_EXTENSIONS
#include "backends/imgui_impl_osx.h"
#endif

#ifdef MERCURY_GRAPHICS_API_VULKAN
#include "llri_vulkan/vk_swapchain.h"
#define VOLK_H_
#include "backends/imgui_impl_vulkan.cpp"
static void mercury_check_vk_result(VkResult err)
{
}
#endif

#ifdef MERCURY_GRAPHICS_API_D3D12
#include "llri_d3d12/d3d12_swapchain.h"

#include "backends/imgui_impl_dx12.cpp"
static ID3D12DescriptorHeap* gImgui_pd3dSrvDescHeap = nullptr;
#endif

#ifdef MERCURY_GRAPHICS_API_WEBGPU
#include "llri_webgpu/webgpu_swapchain.h"

#include "backends/imgui_impl_wgpu.cpp"
#endif

#include <algorithm>

bool gImGuiInitialized = false;
std::vector<mercury::imgui_interface::Screen*> gAllScreens;

void mercury::imgui::initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();
	gAllScreens.reserve(32);
}

void mercury::imgui::shutdown()
{
	if (gImGuiInitialized)
	{
#ifdef MERCURY_GRAPHICS_API_VULKAN
		ImGui_ImplVulkan_Shutdown();
#endif
#ifdef MERCURY_GRAPHICS_API_D3D12
		ImGui_ImplDX12_Shutdown();
#endif
#ifdef MERCURY_GRAPHICS_API_WEBGPU
        ImGui_ImplWGPU_Shutdown();
#endif
#ifdef MERCURY_PLATFORM_ANDROID
		ImGui_ImplAndroid_Shutdown();
#endif
#ifdef MERCURY_PLATFORM_WINDOWS
		ImGui_ImplWin32_Shutdown();
#endif
#ifdef MERCURY_PLATFORM_EMSCRIPTEN
        ImGui_ImplEmscripten_Shutdown();
#endif
	}

	ImGui::DestroyContext();
	gImGuiInitialized = false;
}



bool mercury::imgui_interface::AddScreen(mercury::imgui_interface::Screen* screen)
{
	screen->Activate();
	gAllScreens.push_back(screen);
	return true;
}

bool IsBackendReady()
{
#ifdef MERCURY_GRAPHICS_API_VULKAN
	return gRenderPass != 0;
#endif
#ifdef MERCURY_GRAPHICS_API_D3D12
	return gSwapChain != nullptr;
#endif
#ifdef MERCURY_GRAPHICS_API_WEBGPU
    return gSwapChain != nullptr;
#endif
#ifdef MERCURY_GRAPHICS_API_METAL
    return false;
#endif
	return true;
}

bool InitializeBackend()
{
#ifdef MERCURY_GRAPHICS_API_VULKAN
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = gInstance;
	init_info.PhysicalDevice = gPhysicalDevice;
	init_info.Device = gDevice;
	init_info.QueueFamily = 0;
	init_info.Queue = gMainQueue;
	init_info.PipelineCache = VK_NULL_HANDLE; //TODO: implement pipeline cache
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.CheckVkResultFn = mercury_check_vk_result;
	init_info.MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	init_info.RenderPass = gRenderPass;

	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo dp_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,nullptr };
	dp_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	dp_create_info.maxSets = 1000;
	dp_create_info.pPoolSizes = pool_sizes;
	dp_create_info.poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]);

	vkCreateDescriptorPool(gDevice, &dp_create_info, nullptr, &init_info.DescriptorPool);

	ImGui_ImplVulkan_Init(&init_info);
	ImGui_ImplVulkan_CreateFontsTexture();
#endif

#ifdef MERCURY_GRAPHICS_API_D3D12
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	gDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&gImgui_pd3dSrvDescHeap));

	ImGui_ImplDX12_Init(gDevice, 3, gSwapChainFormat, gImgui_pd3dSrvDescHeap
		, gImgui_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart()
		, gImgui_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	ImGui_ImplDX12_CreateDeviceObjects();
	ImGui_ImplDX12_CreateFontsTexture();
#endif

#ifdef MERCURY_GRAPHICS_API_WEBGPU
    ImGui_ImplWGPU_InitInfo initInfo = {};
    initInfo.Device = gDevice;
    initInfo.RenderTargetFormat = gPrefferedBackbufferFormat;
    initInfo.NumFramesInFlight = 3;
    initInfo.DepthStencilFormat = WGPUTextureFormat::WGPUTextureFormat_Undefined;

    ImGui_ImplWGPU_Init(&initInfo);
#endif
	return true;
}

void mercury::imgui::update()
{

	if (!gImGuiInitialized)
	{
		if (IsBackendReady())
		{
#ifdef MERCURY_PLATFORM_WINDOWS 
			HWND mainWinHandle = static_cast<HWND>(mercury::platform::getMainWindowHandle());
			ImGui_ImplWin32_Init(mainWinHandle);
#endif
#ifdef MERCURY_PLATFORM_LINUX
			auto win = static_cast<xcb_window_t*>(mercury::platform::getMainWindowHandle());
			auto connection = static_cast<xcb_connection_t*>(mercury::platform::getAppInstanceHandle());
			ImGui_ImplX11_Init(connection,win);
#endif
#ifdef MERCURY_PLATFORM_ANDROID
			ImGui_ImplAndroid_Init(static_cast<ANativeWindow*>(mercury::platform::getMainWindowHandle()));
			ImGui::GetIO().FontGlobalScale = 2.0f;
#endif
#ifdef MERCURY_PLATFORM_MACOS
            void* view = mercury::platform::getAppInstanceHandle(); //view
			ImGui_ImplOSX_Init(view);
#endif
#ifdef MERCURY_PLATFORM_EMSCRIPTEN
			ImGui_ImplEmscripten_Init();
#endif
			InitializeBackend();
			gImGuiInitialized = true;
		}
	}


	if (gImGuiInitialized)
	{
#ifdef MERCURY_GRAPHICS_API_VULKAN
		ImGui_ImplVulkan_NewFrame();
#endif
#ifdef MERCURY_GRAPHICS_API_D3D12
		ImGui_ImplDX12_NewFrame();
#endif
#ifdef MERCURY_GRAPHICS_API_WEBGPU
        ImGui_ImplWGPU_NewFrame();
#endif
#ifdef MERCURY_PLATFORM_WINDOWS 
		ImGui_ImplWin32_NewFrame();
#endif
#ifdef MERCURY_PLATFORM_LINUX

		ImGui_ImplX11_NewFrame();
#endif
#ifdef MERCURY_PLATFORM_ANDROID
		ImGui_ImplAndroid_NewFrame();
#endif
#ifdef MERCURY_PLATFORM_MACOS
        void* view = mercury::platform::getAppInstanceHandle(); //view
        ImGui_ImplOSX_NewFrame(view);
#endif
#ifdef MERCURY_PLATFORM_EMSCRIPTEN
        ImGui_ImplEmscripten_Event();
        ImGui_ImplEmscripten_NewFrame();
#endif
		ImGui::NewFrame();

		for (auto* screen : gAllScreens)
		{
			screen->Draw();
		}

		gAllScreens.erase(
			std::remove_if(gAllScreens.begin(), gAllScreens.end(),
				[](const auto o) { return !o->IsActive(); }),
			gAllScreens.end());

		ImGui::ShowDemoWindow(); // Show demo window! :)
		ImGui::Render();
	}
}

void mercury::imgui::render(llri::context& ctx)
{
	if (gImGuiInitialized)
	{
#ifdef MERCURY_GRAPHICS_API_VULKAN
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<VkCommandBuffer>(ctx.impl));
#endif

#ifdef MERCURY_GRAPHICS_API_D3D12
		auto cmdList = static_cast<ID3D12GraphicsCommandList*>(ctx.impl);

		cmdList->SetDescriptorHeaps(1, &gImgui_pd3dSrvDescHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
#endif
#ifdef MERCURY_GRAPHICS_API_WEBGPU
        auto cmdList = static_cast<WGPURenderPassEncoder>(ctx.impl);
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), cmdList);
#endif
	}	
}

