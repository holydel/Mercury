#pragma once
#include "mercury_api.h"
#include <string>
#include <vector>

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

	struct RendererConfig
	{
		i8 adapterID = -1;

#ifdef NDEBUG
		bool enableDebugLayers = false;
#else
		bool enableDebugLayers = true;
#endif
	};

	struct AppConfig
	{
		const char* appName = "NoName Mercury App";

		OutputMode outputMode = OutputMode::Window;
		WindowConfig output;
		EngineConfig engine;
		RendererConfig render;
		
		struct Version
		{
			union
			{
				struct
				{
					u8 major;
					u8 minor;
					u16 patch;
				};

				u32 packed;
			};
		} version = Version{ {{0,0,1}} };

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
		virtual bool Render() { return false; };
		virtual bool PreRender() { return false; };
		virtual bool PostRender() { return false; };

		virtual void OnApplicationClose();

		const std::string& GetRuntimeSettingsValue(const char* u8key);
		const std::vector<std::string>& GetRuntimeSettingsList(const char* u8key);

		void SetRuntimeSettingsValue(const char* u8key, const std::string& value);
		void SetRuntimeSettingsList(const char* u8key, const std::vector<std::string> & value);

		void SetWindowTitle(const char* u8string);
	};
}