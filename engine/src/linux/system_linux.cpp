

#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_LINUX
#include "mercury_log.h"
#include <xcb/xcb.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <atomic>
#include <cstdio>
#include <dlfcn.h>
#include <unordered_map>

using namespace std::chrono_literals;
using namespace mercury;

#include "../application.h"
#include "../platform.h"

using namespace mercury;

xcb_connection_t *gConnection = nullptr;
xcb_window_t gWindow;
const xcb_setup_t *gSetup;
xcb_screen_t *gScreen = nullptr;
xcb_atom_t gWmProtocols;
xcb_atom_t gWmDeleteWin;

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
	auto& config = gApplication->config.output;
	const char* utf8WinCaption = gApplication->config.GetWindowTitle();

	/* Open the connection to the X server */
	gConnection = xcb_connect (NULL, NULL);


	/* Get the first screen */
	gSetup = xcb_get_setup (gConnection);
	xcb_screen_iterator_t   iter   = xcb_setup_roots_iterator (gSetup);
	gScreen = iter.data;

	uint32_t valueList[] = {65535};
	/* Create the window */
	gWindow = xcb_generate_id (gConnection);
	xcb_create_window (gConnection,                    /* Connection          */
					   XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
					   gWindow,                        /* window Id           */
					   gScreen->root,                  /* parent window       */
					   0, 0,                          /* x, y                */
					   1750, 950,                      /* width, height       */
					   10,                            /* border_width        */
					   XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
					   gScreen->root_visual,           /* visual              */
					   XCB_CW_EVENT_MASK, valueList );                     /* masks, not used yet */


	/* Map the window on the screen */
	xcb_map_window (gConnection, gWindow);

	xcb_change_property(
  gConnection,
  XCB_PROP_MODE_REPLACE,
  gWindow,
  XCB_ATOM_WM_NAME,
  XCB_ATOM_STRING,
  8,
  strlen(utf8WinCaption),
  utf8WinCaption);

	/* Make sure commands are sent before we pause so that the window gets shown */
	xcb_flush (gConnection);


}

void platform::destroyMainWindow()
{
	xcb_destroy_window(gConnection, gWindow);
	xcb_disconnect (gConnection);
}

extern bool ImGui_ImplX11_Event(xcb_generic_event_t *event);

void platform::update()
{
	using namespace std::chrono_literals;
	
	xcb_generic_event_t *event;
	xcb_client_message_event_t *cm;

		event = xcb_poll_for_event(gConnection);

		if(event) {
			switch (event->response_type & ~0x80) {
				case XCB_CLIENT_MESSAGE: {
					cm = (xcb_client_message_event_t *)event;
					ImGui_ImplX11_Event(event);

					break;
				}
			}

			free(event);
		}


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
	return static_cast<void*>(&gWindow);
}

void* platform::getAppInstanceHandle()
{
	return static_cast<void*>(gConnection);
}



int utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

int wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

#ifdef MERCURY_GRAPHICS_API_VULKAN
const char* platform::getVulkanLibraryPath()
{
	static const char* libName = u8"libvulkan.so";
	return libName;
}
#endif

#endif
