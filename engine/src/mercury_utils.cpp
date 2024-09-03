#include "mercury_material.h"
#include <cassert>

using namespace mercury;


u32 mercury::utils::GetFormatSizeBits(Format fmt)
{
	switch (fmt)
	{
		case Format::R32G32B32A32_SFLOAT:
			return 128;
		case Format::R32G32B32_SFLOAT:
			return 96;
		case Format::R32G32_SFLOAT:
		case Format::R16G16B16A16_SFLOAT:
			return 64;
		case Format::R16G16B16_SFLOAT:
			return 48;
		case Format::R5G6B5_UNORM_PACK16:
			return 16;
	}

	assert(!!!"Missed Format!");
	return -1;
}