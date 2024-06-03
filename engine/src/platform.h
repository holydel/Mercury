#pragma once
#include "mercury_api.h"
#include <stdint.h>
#include <vector>

namespace mercury
{
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
	}
}


#define LOAD_FUNC_PTR(libHandle,funcPtrOut) funcPtrOut = mercury::platform::getFuncPtr<decltype(funcPtrOut)>(libHandle,#funcPtrOut);
#define M_ASSERT(assert) if(!(assert)) {mercury::platform::fatalFail(#assert);}