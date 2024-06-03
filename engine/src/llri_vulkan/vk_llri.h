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
#endif