#include "vk_swapchain.h"
#include "../platform.h"

VkSurfaceKHR gSurface = nullptr;
VkSwapchainKHR gSwapChain = nullptr;
std::vector<VkSurfaceFormatKHR> support_formats;
std::vector<VkPresentModeKHR> support_present_modes;
VkSurfaceCapabilitiesKHR gSurfaceCaps;

bool llri::swapchain::create(void* nativeWindowHandle)
{

#ifdef MERCURY_PLATFORM_WINDOWS
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = static_cast<HWND>(nativeWindowHandle);
	createInfo.hinstance = static_cast<HINSTANCE>(mercury::platform::getAppInstanceHandle());

	VK_CALL(vkCreateWin32SurfaceKHR(gInstance, &createInfo, nullptr, &gSurface));
#endif

#ifdef MERCURY_PLATFORM_ANDROID
	VkAndroidSurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,nullptr };

	createInfo.flags = 0;
	createInfo.window = static_cast<ANativeWindow*>(nativeWindowHandle);

	vkCreateAndroidSurfaceKHR(gInstance, &createInfo, nullptr, &gSurface);
#endif

	support_formats = EnumerateVulkanObjects(gPhysicalDevice, gSurface, vkGetPhysicalDeviceSurfaceFormatsKHR);
	support_present_modes = EnumerateVulkanObjects(gPhysicalDevice, gSurface, vkGetPhysicalDeviceSurfacePresentModesKHR);

	VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gPhysicalDevice, gSurface, &gSurfaceCaps));


	return true;
}

bool llri::swapchain::destroy()
{
	if (gSwapChain != nullptr)
	{
		vkDestroySwapchainKHR(gDevice, gSwapChain, gGlobalAllocationsCallbacks);
	}
	if (gSurface != nullptr)
	{
		vkDestroySurfaceKHR(gInstance, gSurface, gGlobalAllocationsCallbacks);
	}
	return true;
}