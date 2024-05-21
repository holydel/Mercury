#pragma once

#include "mercury_api.h"

namespace llri
{
	namespace swapchain
	{
		bool create(void* nativeWindowHandle);
		bool destroy();
	}	
}