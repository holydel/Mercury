#pragma once

#include "mercury_api.h"

namespace mercury
{
	namespace imgui_interface
	{
		class Screen
		{
		protected:
			bool is_active = false;
		public:
			virtual void Draw() = 0;

			bool IsActive()
			{
				return is_active;
			};

			void Activate()
			{
				is_active = true;
			}
		};

		bool AddScreen(Screen* screen);
	}
}