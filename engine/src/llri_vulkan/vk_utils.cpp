#include "vk_utils.h"

#ifdef MERCURY_GRAPHICS_API_VULKAN
#include <cassert>

using namespace mercury;

VkFormat vk_utils::GetFormatFromMercuryFormat(mercury::Format fmt)
{
	switch (fmt)
	{
	case Format::R32G32B32A32_SFLOAT:
		return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
	case Format::R32G32B32_SFLOAT:
		return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
	case Format::R16G16B16A16_SFLOAT:
		return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
	case Format::R16G16B16_SFLOAT:
		return VkFormat::VK_FORMAT_R16G16B16_SFLOAT;
	case Format::R5G6B5_UNORM_PACK16:
		return VkFormat::VK_FORMAT_R5G6B5_UNORM_PACK16;
	}

	assert(!!!"Missed Format!");
	return VkFormat::VK_FORMAT_UNDEFINED;
}

#endif