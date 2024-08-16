#pragma once
#include "mercury_api.h"

#ifdef MERCURY_GRAPHICS_API_VULKAN
#include "mercury_material.h"
#include "mercury_vulkan.h"

namespace vk_utils
{
	VkFormat GetFormatFromMercuryFormat(mercury::Format fmt);
}

#endif