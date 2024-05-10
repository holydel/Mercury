#pragma once
#include "mercury_api.h"

namespace mercury
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual bool Initialize();

		virtual bool Shutdown();

		virtual bool Update();

		virtual const char* GetName();
	};
}