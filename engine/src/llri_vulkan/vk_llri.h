#pragma once
#include "../llri.h"

#ifdef MERCURY_GRAPHICS_API_VULKAN
#include "mercury_log.h"
#include "mercury_vulkan.h"

extern VkInstance gInstance;
extern VkAllocationCallbacks* gGlobalAllocationsCallbacks;
extern VkPhysicalDevice gPhysicalDevice;
extern VkDevice gDevice;
extern VmaAllocator gVMAallocator;
extern VkQueue gMainQueue;
extern VkCommandBuffer gCurrentCommandBuffer;

extern std::vector<VkShaderModule> gAllShaderModules;
extern std::vector<VkPipeline> gAllPSOs;
extern std::vector<VkPipelineLayout> gAllLayouts;
extern std::vector<VkBuffer> gAllBuffers;

struct BufferMeta
{
	void* mappedPointer;
	VmaAllocation allocation;
	VmaAllocationInfo allocInfo;
	VkDeviceMemory dedicatedMemory;
	VkDeviceAddress deviceAddress;
};

extern std::vector<BufferMeta> gAllBuffersMeta;

#endif