#pragma once
#include "mercury_api.h"
#include "mercury_imgui_interface.h"

namespace mercury
{
	namespace imgui
	{
		void initialize();
		void shutdown();
		void update();


		void render(void* cbuff);

	}
}
