#include "mercury_api.h"

#ifdef MERCURY_GRAPHICS_API_VULKAN

#define VMA_IMPLEMENTATION

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wnullability-completeness"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#include "mercury_vulkan.h"
#pragma GCC diagnostic pop



#include "../platform.h"
#include "mercury_log.h"
using namespace mercury;

#define VK_DEFINE_FUNCTION( fun ) PFN_##fun fun = nullptr;

//GLOBAL
VK_DEFINE_FUNCTION(vkGetInstanceProcAddr);
VK_DEFINE_FUNCTION(vkEnumerateInstanceVersion);
VK_DEFINE_FUNCTION(vkCreateInstance);
VK_DEFINE_FUNCTION(vkEnumerateInstanceExtensionProperties);
VK_DEFINE_FUNCTION(vkEnumerateInstanceLayerProperties);

//INSTANCE LEVEL
VK_DEFINE_FUNCTION(vkDestroyInstance);
VK_DEFINE_FUNCTION(vkEnumeratePhysicalDevices);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceProperties);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceProperties2);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceProperties2KHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
VK_DEFINE_FUNCTION(vkEnumerateDeviceExtensionProperties);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceFeatures);
VK_DEFINE_FUNCTION(vkCreateDevice);
VK_DEFINE_FUNCTION(vkGetDeviceProcAddr);
VK_DEFINE_FUNCTION(vkCreateDebugUtilsMessengerEXT);
VK_DEFINE_FUNCTION(vkDestroyDebugUtilsMessengerEXT);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfaceFormats2KHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
VK_DEFINE_FUNCTION(vkDestroySurfaceKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceDisplayPropertiesKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceDisplayProperties2KHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceMemoryProperties2);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceSparseImageFormatProperties);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceImageFormatProperties);
VK_DEFINE_FUNCTION(vkEnumeratePhysicalDeviceGroups);
VK_DEFINE_FUNCTION(vkDestroyDescriptorSetLayout);
VK_DEFINE_FUNCTION(vkFreeDescriptorSets);

#ifdef MERCURY_PLATFORM_WINDOWS
VK_DEFINE_FUNCTION(vkCreateWin32SurfaceKHR);
VK_DEFINE_FUNCTION(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif

#ifdef MERCURY_PLATFORM_ANDROID
VK_DEFINE_FUNCTION(vkCreateAndroidSurfaceKHR);
#endif

#ifdef MERCURY_PLATFORM_LINUX
VK_DEFINE_FUNCTION(vkCreateXcbSurfaceKHR);
#endif

#ifdef MERCURY_PLATFORM_MACOS
VK_DEFINE_FUNCTION(vkCreateMetalSurfaceEXT);
#endif
//GLOBAL LEVEL
VK_DEFINE_FUNCTION(vkDestroyDevice);
VK_DEFINE_FUNCTION(vkDeviceWaitIdle);
VK_DEFINE_FUNCTION(vkQueueWaitIdle);
VK_DEFINE_FUNCTION(vkCreateSwapchainKHR);
VK_DEFINE_FUNCTION(vkDestroySwapchainKHR);
VK_DEFINE_FUNCTION(vkGetSwapchainImagesKHR);
VK_DEFINE_FUNCTION(vkAcquireNextImageKHR);
VK_DEFINE_FUNCTION(vkQueueSubmit);
VK_DEFINE_FUNCTION(vkQueuePresentKHR);
VK_DEFINE_FUNCTION(vkGetDeviceQueue);
VK_DEFINE_FUNCTION(vkCreateCommandPool);
VK_DEFINE_FUNCTION(vkAllocateCommandBuffers);
VK_DEFINE_FUNCTION(vkBeginCommandBuffer);
VK_DEFINE_FUNCTION(vkCmdClearColorImage);
VK_DEFINE_FUNCTION(vkCmdPipelineBarrier);
VK_DEFINE_FUNCTION(vkEndCommandBuffer);
VK_DEFINE_FUNCTION(vkFreeCommandBuffers);
VK_DEFINE_FUNCTION(vkDestroyCommandPool);
VK_DEFINE_FUNCTION(vkResetCommandPool);
VK_DEFINE_FUNCTION(vkCreateSemaphore);
VK_DEFINE_FUNCTION(vkDestroySemaphore);
VK_DEFINE_FUNCTION(vkCreateImageView);
VK_DEFINE_FUNCTION(vkDestroyImageView);
VK_DEFINE_FUNCTION(vkCreateRenderPass);
VK_DEFINE_FUNCTION(vkDestroyRenderPass);
VK_DEFINE_FUNCTION(vkCreateFramebuffer);
VK_DEFINE_FUNCTION(vkDestroyFramebuffer);
VK_DEFINE_FUNCTION(vkCmdBeginRenderPass);
VK_DEFINE_FUNCTION(vkCmdEndRenderPass);
VK_DEFINE_FUNCTION(vkCreateFence);
VK_DEFINE_FUNCTION(vkDestroyFence);
VK_DEFINE_FUNCTION(vkWaitForFences);
VK_DEFINE_FUNCTION(vkResetFences);
VK_DEFINE_FUNCTION(vkCreateShaderModule);
VK_DEFINE_FUNCTION(vkDestroyShaderModule);
VK_DEFINE_FUNCTION(vkCreateGraphicsPipelines);
VK_DEFINE_FUNCTION(vkDestroyPipeline);
VK_DEFINE_FUNCTION(vkCreatePipelineLayout);
VK_DEFINE_FUNCTION(vkDestroyPipelineLayout);
VK_DEFINE_FUNCTION(vkCmdBindPipeline);
VK_DEFINE_FUNCTION(vkCmdDraw);
VK_DEFINE_FUNCTION(vkCmdDrawIndexed);
VK_DEFINE_FUNCTION(vkCmdSetViewport)
VK_DEFINE_FUNCTION(vkCmdSetScissor);
VK_DEFINE_FUNCTION(vkCmdPushConstants);
VK_DEFINE_FUNCTION(vkCreateImage);
VK_DEFINE_FUNCTION(vkDestroyImage);
VK_DEFINE_FUNCTION(vkGetImageMemoryRequirements);
VK_DEFINE_FUNCTION(vkAllocateMemory);
VK_DEFINE_FUNCTION(vkFreeMemory);
VK_DEFINE_FUNCTION(vkBindImageMemory);
VK_DEFINE_FUNCTION(vkBindBufferMemory);
VK_DEFINE_FUNCTION(vkCreateBuffer);
VK_DEFINE_FUNCTION(vkDestroyBuffer);
VK_DEFINE_FUNCTION(vkGetBufferDeviceAddress);
VK_DEFINE_FUNCTION(vkGetBufferMemoryRequirements);
VK_DEFINE_FUNCTION(vkGetDeviceBufferMemoryRequirements);
VK_DEFINE_FUNCTION(vkGetDeviceImageMemoryRequirements);
VK_DEFINE_FUNCTION(vkCmdFillBuffer);
VK_DEFINE_FUNCTION(vkInvalidateMappedMemoryRanges);
VK_DEFINE_FUNCTION(vkMapMemory);
VK_DEFINE_FUNCTION(vkUnmapMemory);
VK_DEFINE_FUNCTION(vkFlushMappedMemoryRanges);
VK_DEFINE_FUNCTION(vkCmdCopyImageToBuffer);
VK_DEFINE_FUNCTION(vkCmdCopyBufferToImage);
VK_DEFINE_FUNCTION(vkCreateSampler);
VK_DEFINE_FUNCTION(vkCreateSamplerYcbcrConversion);
VK_DEFINE_FUNCTION(vkDestroySampler);
VK_DEFINE_FUNCTION(vkCreateDescriptorSetLayout);
VK_DEFINE_FUNCTION(vkCreateDescriptorPool);
VK_DEFINE_FUNCTION(vkDestroyDescriptorPool);
VK_DEFINE_FUNCTION(vkCmdBindDescriptorSets);
VK_DEFINE_FUNCTION(vkAllocateDescriptorSets);
VK_DEFINE_FUNCTION(vkUpdateDescriptorSets);
VK_DEFINE_FUNCTION(vkCmdPushDescriptorSetKHR);
VK_DEFINE_FUNCTION(vkCmdBindVertexBuffers);
VK_DEFINE_FUNCTION(vkCmdBindIndexBuffer);
VK_DEFINE_FUNCTION(vkCmdExecuteCommands);
VK_DEFINE_FUNCTION(vkCmdCopyBuffer);
VK_DEFINE_FUNCTION(vkCmdBlitImage);
VK_DEFINE_FUNCTION(vkGetImageSparseMemoryRequirements);
VK_DEFINE_FUNCTION(vkGetImageSparseMemoryRequirements2);
VK_DEFINE_FUNCTION(vkQueueBindSparse);
VK_DEFINE_FUNCTION(vkCreateQueryPool);
VK_DEFINE_FUNCTION(vkDestroyQueryPool);
VK_DEFINE_FUNCTION(vkCreateComputePipelines);
VK_DEFINE_FUNCTION(vkCmdDispatch);
VK_DEFINE_FUNCTION(vkDebugMarkerSetObjectTagEXT);
VK_DEFINE_FUNCTION(vkDebugMarkerSetObjectNameEXT);
VK_DEFINE_FUNCTION(vkCmdDebugMarkerBeginEXT);
VK_DEFINE_FUNCTION(vkCmdDebugMarkerEndEXT);
VK_DEFINE_FUNCTION(vkCmdDebugMarkerInsertEXT);
VK_DEFINE_FUNCTION(vkCmdBeginQuery);
VK_DEFINE_FUNCTION(vkCmdEndQuery);
VK_DEFINE_FUNCTION(vkGetQueryPoolResults);
VK_DEFINE_FUNCTION(vkCmdResetQueryPool);
VK_DEFINE_FUNCTION(vkCmdCopyQueryPoolResults);
VK_DEFINE_FUNCTION(vkBindBufferMemory2);
VK_DEFINE_FUNCTION(vkBindImageMemory2);
VK_DEFINE_FUNCTION(vkGetBufferMemoryRequirements2);
VK_DEFINE_FUNCTION(vkGetImageMemoryRequirements2);

#ifdef MERCURY_PLATFORM_WINDOWS
VK_DEFINE_FUNCTION(vkGetWinrtDisplayNV);
VK_DEFINE_FUNCTION(vkAcquireWinrtDisplayNV);
#endif

static void* libHandle = nullptr;

#define VK_LOAD_GLOBAL_FUNC(func) func = (PFN_##func)vkGetInstanceProcAddr( nullptr, #func)
#define VK_LOAD_INSTANCE_FUNC(func) func = (PFN_##func)vkGetInstanceProcAddr(instance, #func)
#define VK_LOAD_DEVICE_FUNC(func) func = (PFN_##func)vkGetDeviceProcAddr(device,#func)

void LoadVK_Library()
{
#ifdef MERCURY_PLATFORM_WINDOWS
	const char* libName = u8"vulkan-1.dll";
#elif defined(MERCURY_PLATFORM_MACOS)
    const char* libName = u8"libvulkan.1.dylib";
#else
	const char* libName = u8"libvulkan.so";
#endif

	libHandle = platform::loadSharedLibrary(libName);
	M_ASSERT(libHandle != nullptr);

	LOAD_FUNC_PTR(libHandle, vkGetInstanceProcAddr);

	VK_LOAD_GLOBAL_FUNC(vkEnumerateInstanceVersion);
	VK_LOAD_GLOBAL_FUNC(vkCreateInstance);
	VK_LOAD_GLOBAL_FUNC(vkEnumerateInstanceExtensionProperties);
	VK_LOAD_GLOBAL_FUNC(vkEnumerateInstanceLayerProperties);

	auto libFullName = platform::getSharedLibraryFullFilename(libHandle);
	write_log_message("Found vulkan loader: %s", libFullName);
}

void LoadVkInstanceLevelFuncs(VkInstance instance)
{
	VK_LOAD_INSTANCE_FUNC(vkDestroyInstance);
	VK_LOAD_INSTANCE_FUNC(vkEnumeratePhysicalDevices);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceProperties);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceProperties2);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceProperties2KHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceQueueFamilyProperties);
	VK_LOAD_INSTANCE_FUNC(vkEnumerateDeviceExtensionProperties);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceFeatures);
	VK_LOAD_INSTANCE_FUNC(vkCreateDevice);
	VK_LOAD_INSTANCE_FUNC(vkGetDeviceProcAddr);
	VK_LOAD_INSTANCE_FUNC(vkCreateDebugUtilsMessengerEXT);
	VK_LOAD_INSTANCE_FUNC(vkDestroyDebugUtilsMessengerEXT);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceSupportKHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceFormatsKHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceFormats2KHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR);
	VK_LOAD_INSTANCE_FUNC(vkDestroySurfaceKHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceDisplayPropertiesKHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceDisplayProperties2KHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceMemoryProperties);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceMemoryProperties2);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceSparseImageFormatProperties);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceImageFormatProperties);
	VK_LOAD_INSTANCE_FUNC(vkEnumeratePhysicalDeviceGroups);

#ifdef MERCURY_PLATFORM_WINDOWS
	VK_LOAD_INSTANCE_FUNC(vkCreateWin32SurfaceKHR);
	VK_LOAD_INSTANCE_FUNC(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif

#ifdef MERCURY_PLATFORM_ANDROID
	VK_LOAD_INSTANCE_FUNC(vkCreateAndroidSurfaceKHR);
#endif

#ifdef MERCURY_PLATFORM_LINUX
	VK_LOAD_INSTANCE_FUNC(vkCreateXcbSurfaceKHR);
#endif

#ifdef MERCURY_PLATFORM_MACOS
    VK_LOAD_INSTANCE_FUNC(vkCreateMetalSurfaceEXT);
#endif
}

void LoadVkDeviceLevelFuncs(VkDevice device)
{
	VK_LOAD_DEVICE_FUNC(vkDestroyDevice);
	VK_LOAD_DEVICE_FUNC(vkDeviceWaitIdle);
	VK_LOAD_DEVICE_FUNC(vkQueueWaitIdle);
	VK_LOAD_DEVICE_FUNC(vkCreateSwapchainKHR);
	VK_LOAD_DEVICE_FUNC(vkDestroySwapchainKHR);
	VK_LOAD_DEVICE_FUNC(vkGetSwapchainImagesKHR);
	VK_LOAD_DEVICE_FUNC(vkAcquireNextImageKHR);
	VK_LOAD_DEVICE_FUNC(vkQueueSubmit);
	VK_LOAD_DEVICE_FUNC(vkQueuePresentKHR);
	VK_LOAD_DEVICE_FUNC(vkGetDeviceQueue);
	VK_LOAD_DEVICE_FUNC(vkCreateCommandPool);
	VK_LOAD_DEVICE_FUNC(vkAllocateCommandBuffers);
	VK_LOAD_DEVICE_FUNC(vkBeginCommandBuffer);
	VK_LOAD_DEVICE_FUNC(vkCmdClearColorImage);
	VK_LOAD_DEVICE_FUNC(vkCmdPipelineBarrier);
	VK_LOAD_DEVICE_FUNC(vkEndCommandBuffer);
	VK_LOAD_DEVICE_FUNC(vkFreeCommandBuffers);
	VK_LOAD_DEVICE_FUNC(vkDestroyCommandPool);
	VK_LOAD_DEVICE_FUNC(vkResetCommandPool);
	VK_LOAD_DEVICE_FUNC(vkCreateSemaphore);
	VK_LOAD_DEVICE_FUNC(vkDestroySemaphore);
	VK_LOAD_DEVICE_FUNC(vkCreateImageView);
	VK_LOAD_DEVICE_FUNC(vkDestroyImageView);
	VK_LOAD_DEVICE_FUNC(vkCreateRenderPass);
	VK_LOAD_DEVICE_FUNC(vkDestroyRenderPass);
	VK_LOAD_DEVICE_FUNC(vkCreateFramebuffer);
	VK_LOAD_DEVICE_FUNC(vkDestroyFramebuffer);
	VK_LOAD_DEVICE_FUNC(vkCmdBeginRenderPass);
	VK_LOAD_DEVICE_FUNC(vkCmdEndRenderPass);
	VK_LOAD_DEVICE_FUNC(vkCreateFence);
	VK_LOAD_DEVICE_FUNC(vkDestroyFence);
	VK_LOAD_DEVICE_FUNC(vkWaitForFences);
	VK_LOAD_DEVICE_FUNC(vkResetFences);
	VK_LOAD_DEVICE_FUNC(vkCreateShaderModule);
	VK_LOAD_DEVICE_FUNC(vkDestroyShaderModule);
	VK_LOAD_DEVICE_FUNC(vkCreateGraphicsPipelines);
	VK_LOAD_DEVICE_FUNC(vkDestroyPipeline);
	VK_LOAD_DEVICE_FUNC(vkCreatePipelineLayout);
	VK_LOAD_DEVICE_FUNC(vkDestroyPipelineLayout);
	VK_LOAD_DEVICE_FUNC(vkCmdBindPipeline);
	VK_LOAD_DEVICE_FUNC(vkCmdDraw);
	VK_LOAD_DEVICE_FUNC(vkCmdDrawIndexed);
	VK_LOAD_DEVICE_FUNC(vkCmdSetViewport);
	VK_LOAD_DEVICE_FUNC(vkCmdSetScissor);
	VK_LOAD_DEVICE_FUNC(vkCmdPushConstants);
	VK_LOAD_DEVICE_FUNC(vkCreateImage);
	VK_LOAD_DEVICE_FUNC(vkDestroyImage);
	VK_LOAD_DEVICE_FUNC(vkGetImageMemoryRequirements);
	VK_LOAD_DEVICE_FUNC(vkAllocateMemory);
	VK_LOAD_DEVICE_FUNC(vkFreeMemory);
	VK_LOAD_DEVICE_FUNC(vkBindImageMemory);
	VK_LOAD_DEVICE_FUNC(vkBindBufferMemory);
	VK_LOAD_DEVICE_FUNC(vkCreateBuffer);
	VK_LOAD_DEVICE_FUNC(vkDestroyBuffer);
	VK_LOAD_DEVICE_FUNC(vkGetBufferDeviceAddress);
	VK_LOAD_DEVICE_FUNC(vkGetBufferMemoryRequirements);
	VK_LOAD_DEVICE_FUNC(vkGetDeviceBufferMemoryRequirements);
	VK_LOAD_DEVICE_FUNC(vkGetDeviceImageMemoryRequirements);
	VK_LOAD_DEVICE_FUNC(vkCmdFillBuffer);
	VK_LOAD_DEVICE_FUNC(vkInvalidateMappedMemoryRanges);
	VK_LOAD_DEVICE_FUNC(vkMapMemory);
	VK_LOAD_DEVICE_FUNC(vkUnmapMemory);
	VK_LOAD_DEVICE_FUNC(vkFlushMappedMemoryRanges);
	VK_LOAD_DEVICE_FUNC(vkCmdCopyImageToBuffer);
	VK_LOAD_DEVICE_FUNC(vkCmdCopyBufferToImage);
	VK_LOAD_DEVICE_FUNC(vkCreateSampler);
	VK_LOAD_DEVICE_FUNC(vkCreateSamplerYcbcrConversion);
	VK_LOAD_DEVICE_FUNC(vkDestroySampler);
	VK_LOAD_DEVICE_FUNC(vkCreateDescriptorSetLayout);
	VK_LOAD_DEVICE_FUNC(vkCreateDescriptorPool);
	VK_LOAD_DEVICE_FUNC(vkDestroyDescriptorPool);
	VK_LOAD_DEVICE_FUNC(vkCmdBindDescriptorSets);
	VK_LOAD_DEVICE_FUNC(vkAllocateDescriptorSets);
	VK_LOAD_DEVICE_FUNC(vkUpdateDescriptorSets);
	VK_LOAD_DEVICE_FUNC(vkCmdPushDescriptorSetKHR);
	VK_LOAD_DEVICE_FUNC(vkCmdBindVertexBuffers);
	VK_LOAD_DEVICE_FUNC(vkCmdBindIndexBuffer);
	VK_LOAD_DEVICE_FUNC(vkCmdExecuteCommands);
	VK_LOAD_DEVICE_FUNC(vkCmdCopyBuffer);
	VK_LOAD_DEVICE_FUNC(vkCmdBlitImage);
	VK_LOAD_DEVICE_FUNC(vkGetImageSparseMemoryRequirements);
	VK_LOAD_DEVICE_FUNC(vkGetImageSparseMemoryRequirements2);
	VK_LOAD_DEVICE_FUNC(vkQueueBindSparse);
	VK_LOAD_DEVICE_FUNC(vkCreateQueryPool);
	VK_LOAD_DEVICE_FUNC(vkDestroyQueryPool);
	VK_LOAD_DEVICE_FUNC(vkCreateComputePipelines);
	VK_LOAD_DEVICE_FUNC(vkCmdDispatch);
	VK_LOAD_DEVICE_FUNC(vkDestroyDescriptorSetLayout);
	VK_LOAD_DEVICE_FUNC(vkFreeDescriptorSets);
	VK_LOAD_DEVICE_FUNC(vkDebugMarkerSetObjectTagEXT);
	VK_LOAD_DEVICE_FUNC(vkDebugMarkerSetObjectNameEXT);
	VK_LOAD_DEVICE_FUNC(vkCmdDebugMarkerBeginEXT);
	VK_LOAD_DEVICE_FUNC(vkCmdDebugMarkerEndEXT);
	VK_LOAD_DEVICE_FUNC(vkCmdDebugMarkerInsertEXT);
	VK_LOAD_DEVICE_FUNC(vkCmdBeginQuery);
	VK_LOAD_DEVICE_FUNC(vkCmdEndQuery);
	VK_LOAD_DEVICE_FUNC(vkGetQueryPoolResults);
	VK_LOAD_DEVICE_FUNC(vkCmdResetQueryPool);
	VK_LOAD_DEVICE_FUNC(vkCmdCopyQueryPoolResults);
	VK_LOAD_DEVICE_FUNC(vkBindBufferMemory2);
	VK_LOAD_DEVICE_FUNC(vkBindImageMemory2);
	VK_LOAD_DEVICE_FUNC(vkGetBufferMemoryRequirements2);
	VK_LOAD_DEVICE_FUNC(vkGetImageMemoryRequirements2);

#ifdef MERCURY_PLATFORM_WINDOWS
	VK_LOAD_DEVICE_FUNC(vkGetWinrtDisplayNV);
	VK_LOAD_DEVICE_FUNC(vkAcquireWinrtDisplayNV);
#endif
}

const char* VkResultToString(VkResult res)
{
	if (res != VK_SUCCESS)
	{
		mercury::write_log_message("err");
	}
	return "unimplemented!";
}

void ShutdownVK_Library()
{
	platform::unloadSharedLibrary(libHandle);
}

#endif //MERCURY_GRAPHICS_API_VULKAN
