#include "mercury_api.h"

#include "mercury_imgui.h"
#include "platform.h"

#include "imgui.h"


#ifdef MERCURY_PLATFORM_WINDOWS
#include "backends/imgui_impl_win32.cpp"
#endif

#ifdef MERCURY_PLATFORM_ANDROID
#include "backends/imgui_impl_android.cpp"
#endif

#ifdef MERCURY_PLATFORM_LINUX
#include "third_party/imgui_impl_x11.cpp"
#endif

#ifdef MERCURY_PLATFORM_MACOS
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

bool gImGuiInitialized = false;

void mercury::imgui::initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

}

void mercury::imgui::shutdown()
{
#ifdef MERCURY_PLATFORM_ANDROID
	ImGui_ImplAndroid_Shutdown();	
#endif
#ifdef MERCURY_GRAPHICS_API_VULKAN
	ImGui_ImplVulkan_Shutdown();
#endif
	ImGui::DestroyContext();
	gImGuiInitialized = false;
}

void mercury::imgui::update()
{

#ifdef MERCURY_GRAPHICS_API_VULKAN
	if (!gImGuiInitialized)
	{
		if (gRenderPass != 0)
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
			dp_create_info.flags = 0;
			dp_create_info.maxSets = 1000;
			dp_create_info.pPoolSizes = pool_sizes;
			dp_create_info.poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]);

			vkCreateDescriptorPool(gDevice, &dp_create_info, nullptr, &init_info.DescriptorPool);

			ImGui_ImplVulkan_Init(&init_info);
			ImGui_ImplVulkan_CreateFontsTexture();
			gImGuiInitialized = true;
		}
	}

#endif

	if (gImGuiInitialized)
	{
#ifdef MERCURY_GRAPHICS_API_VULKAN
		ImGui_ImplVulkan_NewFrame();
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
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(); // Show demo window! :)
		ImGui::Render();
	}
}

void mercury::imgui::render(void* cbuff)
{
	if (gImGuiInitialized)
	{
#ifdef MERCURY_GRAPHICS_API_VULKAN
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<VkCommandBuffer>(cbuff));
#endif
	}	
}

