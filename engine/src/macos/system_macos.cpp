

#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_MACOS
#include "mercury_log.h"

#include <dlfcn.h>
#include <unordered_map>

using namespace std::chrono_literals;
using namespace mercury;

#include "../application.h"
#include "../platform.h"
#include <thread>
#include "cocoa_wrapper.h"

using namespace mercury;

void platform::initialize()
{
}

void platform::shutdown()
{

}

//console main
int main()
{
	ApplicationRun();
	return 0;
}


void platform::createMainWindow()
{
    createCocoaWindow();
}

void platform::destroyMainWindow()
{

}


void platform::update()
{
	using namespace std::chrono_literals;

	std::this_thread::sleep_for((1ms));
}

int platform::utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return 0;// ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, static_cast<int>(strlen(utf8Str)), outBuff, buffLen);
}

int platform::wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return 0;// ::WideCharToMultiByte(CP_UTF8, 0, wideStr, static_cast<int>(wcslen(wideStr)), outBuff, buffLen, nullptr, nullptr);
}

long platform::interlockedAdd32(volatile long* value, long add)
{
	return __sync_fetch_and_add(value, add);
}

void* platform::alignedMalloc(int size, int align)
{
	return aligned_alloc(size, align);
}

void platform::alignedFree(void* ptr)
{
	free(ptr);
}

void* platform::loadSharedLibrary(const char* utf8libname)
{
	return dlopen(utf8libname, RTLD_NOW);
}

bool platform::unloadSharedLibrary(void* library)
{
	dlclose(library);
	return true;
}

std::unordered_map<void*, void*> ptrToLibHandleMap;

void* platform::getFuncPtrImpl(void* library, const char* funcName)
{
	auto result = dlsym(library, funcName);

	ptrToLibHandleMap[library] = result;
	return result;
}

void platform::fatalFail(const char* reason)
{
	mercury::write_log_message("%s\n", reason);
	//DebugBreak();
}

const char* platform::getSharedLibraryFullFilename(void* libHandle)
{
	static Dl_info dl_info = {};
	dladdr(ptrToLibHandleMap[libHandle], &dl_info);

	return dl_info.dli_fname;
}

void platform::outputToDebugConsole(const char* text)
{
	printf("%s", text);
}

void platform::outputToLogFile(const char* text)
{

}

void* platform::getMainWindowHandle()
{
	return 0;
}

void* platform::getAppInstanceHandle()
{
	return 0;
}



int utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

int wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

#endif
