#pragma once

#include "mercury_api.h"

namespace mercury
{
	namespace imgui_interface
	{
		class Screen
		{
		public:
			virtual void Draw() = 0;

			virtual bool IsActive() = 0;

		};

		bool AddScreen(Screen* screen);
	}
}