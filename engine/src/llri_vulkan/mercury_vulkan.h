#pragma once

#include "mercury_api.h"
#include "../platform.h"

#define VK_NO_PROTOTYPES 1

#ifdef MERCURY_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#ifdef MERCURY_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR 1
#endif

#define VK_ENABLE_BETA_EXTENSIONS 1

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>

//VMA
#include "vk_mem_alloc.h"
#include <vector>

void LoadVK_Library();
void LoadVkInstanceLevelFuncs(VkInstance instance);
void LoadVkDeviceLevelFuncs(VkDevice device);
void ShutdownVK_Library();

const char* VkResultToString(VkResult res);

#define VK_CALL(func) {VkResult res = (func); if(res < 0){ mercury::platform::fatalFail( VkResultToString(res) ); } }

#define VK_DECLARE_FUNCTION( fun ) extern PFN_##fun fun;

VK_DECLARE_FUNCTION(vkGetInstanceProcAddr);
VK_DECLARE_FUNCTION(vkEnumerateInstanceVersion);
VK_DECLARE_FUNCTION(vkCreateInstance);
VK_DECLARE_FUNCTION(vkEnumerateInstanceExtensionProperties);
VK_DECLARE_FUNCTION(vkEnumerateInstanceLayerProperties);
VK_DECLARE_FUNCTION(vkDestroyInstance);
VK_DECLARE_FUNCTION(vkEnumeratePhysicalDevices);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceProperties);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceProperties2);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceProperties2KHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
VK_DECLARE_FUNCTION(vkEnumerateDeviceExtensionProperties);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceFeatures);
VK_DECLARE_FUNCTION(vkCreateDevice);
VK_DECLARE_FUNCTION(vkGetDeviceProcAddr);
VK_DECLARE_FUNCTION(vkCreateDebugUtilsMessengerEXT);
VK_DECLARE_FUNCTION(vkDestroyDebugUtilsMessengerEXT);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceFormats2KHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
VK_DECLARE_FUNCTION(vkDestroySurfaceKHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceDisplayPropertiesKHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceDisplayProperties2KHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceMemoryProperties2);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceSparseImageFormatProperties);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceImageFormatProperties);
VK_DECLARE_FUNCTION(vkEnumeratePhysicalDeviceGroups);

#ifdef MERCURY_PLATFORM_WINDOWS
VK_DECLARE_FUNCTION(vkCreateWin32SurfaceKHR);
VK_DECLARE_FUNCTION(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif

#ifdef MERCURY_PLATFORM_ANDROID
VK_DECLARE_FUNCTION(vkCreateAndroidSurfaceKHR);
#endif

//DEVICE LEVEL FUNCS
VK_DECLARE_FUNCTION(vkDestroyDevice);
VK_DECLARE_FUNCTION(vkDeviceWaitIdle);
VK_DECLARE_FUNCTION(vkQueueWaitIdle);
VK_DECLARE_FUNCTION(vkCreateSwapchainKHR);
VK_DECLARE_FUNCTION(vkDestroySwapchainKHR);
VK_DECLARE_FUNCTION(vkGetSwapchainImagesKHR);
VK_DECLARE_FUNCTION(vkAcquireNextImageKHR);
VK_DECLARE_FUNCTION(vkQueueSubmit);
VK_DECLARE_FUNCTION(vkQueuePresentKHR);
VK_DECLARE_FUNCTION(vkGetDeviceQueue);
VK_DECLARE_FUNCTION(vkCreateCommandPool);
VK_DECLARE_FUNCTION(vkAllocateCommandBuffers);
VK_DECLARE_FUNCTION(vkBeginCommandBuffer);
VK_DECLARE_FUNCTION(vkCmdClearColorImage);
VK_DECLARE_FUNCTION(vkCmdPipelineBarrier);
VK_DECLARE_FUNCTION(vkEndCommandBuffer);
VK_DECLARE_FUNCTION(vkFreeCommandBuffers);
VK_DECLARE_FUNCTION(vkDestroyCommandPool);
VK_DECLARE_FUNCTION(vkResetCommandPool);
VK_DECLARE_FUNCTION(vkCreateSemaphore);
VK_DECLARE_FUNCTION(vkDestroySemaphore);
VK_DECLARE_FUNCTION(vkCreateImageView);
VK_DECLARE_FUNCTION(vkDestroyImageView);
VK_DECLARE_FUNCTION(vkCreateRenderPass);
VK_DECLARE_FUNCTION(vkDestroyRenderPass);
VK_DECLARE_FUNCTION(vkCreateFramebuffer);
VK_DECLARE_FUNCTION(vkDestroyFramebuffer);
VK_DECLARE_FUNCTION(vkCmdBeginRenderPass);
VK_DECLARE_FUNCTION(vkCmdEndRenderPass);
VK_DECLARE_FUNCTION(vkCreateFence);
VK_DECLARE_FUNCTION(vkDestroyFence);
VK_DECLARE_FUNCTION(vkWaitForFences);
VK_DECLARE_FUNCTION(vkResetFences);
VK_DECLARE_FUNCTION(vkCreateShaderModule);
VK_DECLARE_FUNCTION(vkDestroyShaderModule);
VK_DECLARE_FUNCTION(vkCreateGraphicsPipelines);
VK_DECLARE_FUNCTION(vkDestroyPipeline);
VK_DECLARE_FUNCTION(vkCreatePipelineLayout);
VK_DECLARE_FUNCTION(vkDestroyPipelineLayout);
VK_DECLARE_FUNCTION(vkCmdBindPipeline);
VK_DECLARE_FUNCTION(vkCmdDraw);
VK_DECLARE_FUNCTION(vkCmdDrawIndexed);
VK_DECLARE_FUNCTION(vkCmdSetViewport)
VK_DECLARE_FUNCTION(vkCmdSetScissor);
VK_DECLARE_FUNCTION(vkCmdPushConstants);
VK_DECLARE_FUNCTION(vkCreateImage);
VK_DECLARE_FUNCTION(vkDestroyImage);
VK_DECLARE_FUNCTION(vkGetImageMemoryRequirements);
VK_DECLARE_FUNCTION(vkAllocateMemory);
VK_DECLARE_FUNCTION(vkFreeMemory);
VK_DECLARE_FUNCTION(vkBindImageMemory);
VK_DECLARE_FUNCTION(vkBindBufferMemory);
VK_DECLARE_FUNCTION(vkCreateBuffer);
VK_DECLARE_FUNCTION(vkDestroyBuffer);
VK_DECLARE_FUNCTION(vkGetBufferDeviceAddress);
VK_DECLARE_FUNCTION(vkGetBufferMemoryRequirements);
VK_DECLARE_FUNCTION(vkGetDeviceBufferMemoryRequirements);
VK_DECLARE_FUNCTION(vkGetDeviceImageMemoryRequirements);
VK_DECLARE_FUNCTION(vkCmdFillBuffer);
VK_DECLARE_FUNCTION(vkInvalidateMappedMemoryRanges);
VK_DECLARE_FUNCTION(vkMapMemory);
VK_DECLARE_FUNCTION(vkUnmapMemory);
VK_DECLARE_FUNCTION(vkFlushMappedMemoryRanges);
VK_DECLARE_FUNCTION(vkCmdCopyImageToBuffer);
VK_DECLARE_FUNCTION(vkCmdCopyBufferToImage);
VK_DECLARE_FUNCTION(vkCreateSampler);
VK_DECLARE_FUNCTION(vkCreateSamplerYcbcrConversion);
VK_DECLARE_FUNCTION(vkDestroySampler);
VK_DECLARE_FUNCTION(vkCreateDescriptorSetLayout);
VK_DECLARE_FUNCTION(vkCreateDescriptorPool);
VK_DECLARE_FUNCTION(vkDestroyDescriptorPool);
VK_DECLARE_FUNCTION(vkCmdBindDescriptorSets);
VK_DECLARE_FUNCTION(vkAllocateDescriptorSets);
VK_DECLARE_FUNCTION(vkUpdateDescriptorSets);
VK_DECLARE_FUNCTION(vkCmdPushDescriptorSetKHR);
VK_DECLARE_FUNCTION(vkCmdBindVertexBuffers);
VK_DECLARE_FUNCTION(vkCmdBindIndexBuffer);
VK_DECLARE_FUNCTION(vkCmdExecuteCommands);
VK_DECLARE_FUNCTION(vkCmdCopyBuffer);
VK_DECLARE_FUNCTION(vkCmdBlitImage);
VK_DECLARE_FUNCTION(vkGetImageSparseMemoryRequirements);
VK_DECLARE_FUNCTION(vkGetImageSparseMemoryRequirements2);
VK_DECLARE_FUNCTION(vkQueueBindSparse);
VK_DECLARE_FUNCTION(vkCreateQueryPool);
VK_DECLARE_FUNCTION(vkDestroyQueryPool);
VK_DECLARE_FUNCTION(vkCreateComputePipelines);
VK_DECLARE_FUNCTION(vkCmdDispatch);
VK_DECLARE_FUNCTION(vkDestroyDescriptorSetLayout);
VK_DECLARE_FUNCTION(vkFreeDescriptorSets);
VK_DECLARE_FUNCTION(vkDebugMarkerSetObjectTagEXT);
VK_DECLARE_FUNCTION(vkDebugMarkerSetObjectNameEXT);
VK_DECLARE_FUNCTION(vkCmdDebugMarkerBeginEXT);
VK_DECLARE_FUNCTION(vkCmdDebugMarkerEndEXT);
VK_DECLARE_FUNCTION(vkCmdDebugMarkerInsertEXT);
VK_DECLARE_FUNCTION(vkCmdBeginQuery);
VK_DECLARE_FUNCTION(vkCmdEndQuery);
VK_DECLARE_FUNCTION(vkGetQueryPoolResults);
VK_DECLARE_FUNCTION(vkCmdResetQueryPool);
VK_DECLARE_FUNCTION(vkCmdCopyQueryPoolResults);

VK_DECLARE_FUNCTION(vkBindBufferMemory2);
VK_DECLARE_FUNCTION(vkBindImageMemory2);
VK_DECLARE_FUNCTION(vkGetBufferMemoryRequirements2);
VK_DECLARE_FUNCTION(vkGetImageMemoryRequirements2);

#ifdef MERCURY_PLATFORM_WINDOWS
VK_DECLARE_FUNCTION(vkGetWinrtDisplayNV);
VK_DECLARE_FUNCTION(vkAcquireWinrtDisplayNV);
#endif

template <typename HostObj, typename T, typename U>
std::vector<T> EnumerateVulkanObjects(HostObj host, U object, VkResult(VKAPI_CALL* pfunc) (HostObj host, U object, uint32_t* cnt, T* objs))
{
	uint32_t count = 0;
	VkResult res = pfunc(host, object, &count, nullptr);
	uint32_t remaining = count;

	if (res == VK_SUCCESS && count > 0)
	{
		std::vector<T> result(count);

		VkResult completeStatus = VK_INCOMPLETE;

		while (completeStatus == VK_INCOMPLETE)
		{
			completeStatus = pfunc(host, object, &remaining, result.data() + (count - remaining));

			if (completeStatus < 0) // error
			{
				break;
			}
		}

		return result;
	}

	return std::vector<T>(0);
}

template <typename HostObj, typename T>
std::vector<T> EnumerateVulkanObjects(HostObj host, VkResult(VKAPI_CALL* pfunc) (HostObj host, uint32_t* cnt, T* objs))
{
	uint32_t count = 0;
	VkResult res = pfunc(host, &count, nullptr);
	uint32_t remaining = count;

	if (res == VK_SUCCESS && count > 0)
	{
		std::vector<T> result(count);

		VkResult completeStatus = VK_INCOMPLETE;

		while (completeStatus == VK_INCOMPLETE)
		{
			completeStatus = pfunc(host, &remaining, result.data() + (count - remaining));

			if (completeStatus < 0) // error
			{
				break;
			}
		}

		return result;
	}

	return std::vector<T>(0);
}

template <typename T>
std::vector<T> EnumerateVulkanObjects(VkResult(VKAPI_CALL* pfunc) (const char* layerName, uint32_t* cnt, T* objs))
{
	uint32_t count = 0;
	VkResult res = pfunc(nullptr, &count, nullptr);
	uint32_t remaining = count;

	if (res == VK_SUCCESS && count > 0)
	{
		std::vector<T> result(count);

		VkResult completeStatus = VK_INCOMPLETE;

		while (completeStatus == VK_INCOMPLETE)
		{
			completeStatus = pfunc(nullptr, &remaining, result.data() + (count - remaining));

			if (completeStatus < 0) // error
			{
				break;
			}
		}

		return result;
	}

	return std::vector<T>(0);
}

template <typename T>
std::vector<T> EnumerateVulkanObjects(VkResult(VKAPI_CALL* pfunc) (uint32_t* cnt, T* objs))
{
	uint32_t count = 0;
	VkResult res = pfunc(&count, nullptr);
	uint32_t remaining = count;

	if (res == VK_SUCCESS && count > 0)
	{
		std::vector<T> result(count);

		VkResult completeStatus = VK_INCOMPLETE;

		while (completeStatus == VK_INCOMPLETE)
		{
			completeStatus = pfunc(&remaining, result.data() + (count - remaining));

			if (completeStatus < 0) // error
			{
				break;
			}
		}

		return result;
	}

	return std::vector<T>(0);
}