#pragma once
#include "mercury_api.h"

namespace mercury
{
	enum class OutputMode
	{
		Headless,
		VR,
		Window, //default - also borderless fullscreen window 
		Display
	};

	struct WindowConfig
	{
		enum class WindowMode
		{
			Windowed,
			BorderlessFullscreen,
			ExclusiveFullscreen //tried to eliminate the compositor's delay
		};

		int width = 800;
		int height = 600;
		const char* title = nullptr; //when nullptr - get name from AppConfig
        WindowMode mode = WindowMode::Windowed;
		
	};

	struct EngineConfig
	{
		bool EnableIMGUI = false;
		bool EnableSound = false;
		bool EnableBulletPhysics = false;
	};

	struct AppConfig
	{
		const char* appName = "NoName Mercury App";

		OutputMode outputMode = OutputMode::Window;
		WindowConfig output;
		EngineConfig engine;

		const char* GetWindowTitle()
		{
			return output.title ? output.title : appName;
		}
	};

	class Application
	{
	public:
		AppConfig config;

		Application();
		virtual ~Application();

		virtual bool Initialize();

		virtual bool Shutdown();

		virtual bool Update();

		virtual void OnApplicationClose();
	};
}