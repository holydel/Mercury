#pragma once
#include "mercury_api.h"

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
