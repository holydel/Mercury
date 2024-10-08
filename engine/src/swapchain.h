#pragma once

#include "mercury_api.h"

namespace llri
{
	namespace swapchain
	{
		bool create(void* nativeWindowHandle);
		bool destroy();

		void setBackGroundColor(float r, float g, float b, float a);
		bool update();

		void resize(mercury::u32 newWidth, mercury::u32 newHeight);
	}	
}