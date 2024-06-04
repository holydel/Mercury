#pragma once

#include "mercury_api.h"
#include "context.h"

namespace mercury
{
	namespace engine
	{
		bool initialize();
		bool update();
		void shutdown();

		void renderCallback(llri::context ctx);
	}
}