#pragma once
#include "mercury_api.h"
#include <stdint.h>
#include <vector>
#include <string>

namespace mercury
{
#ifdef MERCURY_XR_API_OPENXR
	struct OpenXRRuntimeInfo
	{
		bool hasRuntime = false;

		std::string file_format_version = "";
		std::string library_path = "";
		std::string library_fullpath = "";
		std::string name = "";

		bool isValveRuntime = false;
	};

	struct OpenXRLayerInfo
	{
		std::string name;
		std::string library_path = "";
		std::string library_fullpath = "";
		std::string description = "";
		std::string xrGetInstanceProcAddrFuncName = "xrGetInstanceProcAddr";
		std::string xrNegotiateLoaderApiLayerInterfaceFuncName = "xrNegotiateLoaderApiLayerInterface";
	};

	struct OpenXRLayersInfo
	{
		std::vector<OpenXRLayerInfo> implicitLayers;
		std::vector<OpenXRLayerInfo> explicitLayers;
	};
#endif

	namespace platform
	{
		void initialize();
		void shutdown();
		void update();

		int utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen);
		int wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen);

		template <size_t size>
		int utf8toWide(const char* utf8Str, wchar_t outBuff[size])
		{
			return utf8ToWide(utf8Str, outBuff, static_cast<int>(size));
		}

		template <size_t size>
		int wideToUtf8(const wchar_t* wideStr, char outBuff[size])
		{
			return wideToUtf8(wideStr, outBuff, static_cast<int>(size));
		}

#ifdef MERCURY_DESKTOP
		void createMainWindow();
		void destroyMainWindow();
#endif

		long interlockedAdd32(volatile long* value, long add);
		void* alignedMalloc(int size, int align);
		void alignedFree(void* ptr);

		void* loadSharedLibrary(const char* utf8libname);
        const char* getSharedLibraryFullFilename(void* libHandle);

		bool unloadSharedLibrary(void* library);
		void* getFuncPtrImpl(void* library, const char* funcName);

		template <typename T>
		T getFuncPtr(void* library, const char* funcName)
		{
			return reinterpret_cast<T>(getFuncPtrImpl(library, funcName));

		}

		void fatalFail(const char* reason);

		void outputToDebugConsole(const char* text);
		void outputToLogFile(const char* text);

		inline void outputToDebugConsole(const wchar_t* text)
		{
			char buff[1024];
			wideToUtf8(text, buff,1024);
			outputToDebugConsole(buff);
		}

		inline void outputToLogFile(const wchar_t* text)
		{
			char buff[1024];
			wideToUtf8(text, buff, 1024);
			outputToLogFile(buff);
		}

		void* getMainWindowHandle();
		void* getAppInstanceHandle();

		const char* getApplicationDataPath();

		void setMainWindowTitle(const char* u8string);

#ifdef MERCURY_GRAPHICS_API_VULKAN
		const char* getVulkanLibraryPath();
#endif
#ifdef MERCURY_XR_API_OPENXR
		const OpenXRRuntimeInfo& getOpenXRRuntimeInfo();
		const OpenXRLayersInfo& getOpenXRLayersInfo();
#endif
	}
}

#define LOAD_FUNC_BY_NAME(libHandle,funcPtrOut,funcName) funcPtrOut = mercury::platform::getFuncPtr<decltype(funcPtrOut)>(libHandle,funcName);

#define LOAD_FUNC_PTR(libHandle,funcPtrOut) LOAD_FUNC_BY_NAME(libHandle,funcPtrOut,#funcPtrOut)

#define M_ASSERT(assert) if(!(assert)) {mercury::platform::fatalFail(#assert);}