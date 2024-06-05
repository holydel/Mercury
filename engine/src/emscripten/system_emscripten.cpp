#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_EMSCRIPTEN
#include "mercury_log.h"
#include "../application.h"
#include "../platform.h"
#include "../engine.h"

#include <emscripten.h>
#include <emscripten/html5.h>
#include "emscripten/html5_webgpu.h"

using namespace mercury;

void platform::initialize()
{
	mercury::write_log_message("platform EMSCRIPTEN initialize");

}

void platform::shutdown()
{
	mercury::write_log_message("platform EMSCRIPTEN shutdown");

}

EM_BOOL one_iter(double time, void* userData) {
    // Can render to the screen here, etc.
    if(!gApplication->Update())
    {
        puts("exit");
    }
    engine::update();
    // Return true to keep the loop running.
    return EM_TRUE;
}

float emMouseX = 0;
float emMouseY = 0;
bool emMouseDown[5] = {false};

int MouseEvent(int eventType, const EmscriptenMouseEvent *mouseEvent __attribute__((nonnull)), void *userData)
{
    if(EMSCRIPTEN_EVENT_CLICK == eventType)
    {
    }

    if(EMSCRIPTEN_EVENT_MOUSEDOWN == eventType)
    {
        emMouseDown[mouseEvent->button] = true;
    }

    if(EMSCRIPTEN_EVENT_MOUSEUP == eventType)
    {
        emMouseDown[mouseEvent->button] = false;
    }

    if(EMSCRIPTEN_EVENT_MOUSEMOVE == eventType)
    {
        //printf("canvasX: %d\n",mouseEvent->canvasX);
        //printf("canvasY: %d\n",mouseEvent->canvasY);

        //printf("targetX: %d\n",mouseEvent->targetX);
        //printf("targetY: %d\n",mouseEvent->targetY);

        //printf("clientX: %d\n",mouseEvent->clientX);
        //printf("clientY: %d\n",mouseEvent->clientY);

        emMouseX = mouseEvent->targetX;
        emMouseY = mouseEvent->targetY;
    }

    return 0;
}



//console main
int main()
{
    mercury::write_log_message("mercury MAIN");

    engine::initialize();
    gApplication->Initialize();

    //em_mouse_callback_func(MouseEvent);

    emscripten_set_mousemove_callback("canvas", nullptr, true, MouseEvent);
    emscripten_set_mousedown_callback("canvas", nullptr, true, MouseEvent);
    emscripten_set_mouseup_callback("canvas", nullptr, true, MouseEvent);

    emscripten_request_animation_frame_loop(one_iter, 0);
	return 0;
}

void platform::createMainWindow()
{

}

void platform::destroyMainWindow()
{

}
extern WGPUSwapChain gSwapChain;

void* platform::getMainWindowHandle()
{
	return gSwapChain;
}

void* platform::getAppInstanceHandle()
{
	return nullptr;
}

void platform::update()
{

}

//int xc_platform::UTF8ToWideChar(const char* utf8string, wchar_t* out_buff)
//{
//	int numCharsConverted = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, out_buff, 65536);
//	return numCharsConverted;
//}



int platform::utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return 0;
}

int platform::wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return 0;
}

long platform::interlockedAdd32(volatile long* value, long add)
{
	return 0;
}

void* platform::alignedMalloc(int size, int align)
{
	return nullptr;
}

void platform::alignedFree(void* ptr)
{

}

void* platform::loadSharedLibrary(const char* utf8libname)
{
	return nullptr;
}

bool platform::unloadSharedLibrary(void* library)
{
	return true;
}

void* platform::getFuncPtrImpl(void* library, const char* funcName)
{
	return nullptr;
}

void platform::fatalFail(const char* reason)
{
	mercury::write_log_message("%s\n", reason);
}

const char* platform::getSharedLibraryFullFilename(void* libHandle)
{
    return nullptr;
}

void platform::outputToDebugConsole(const char* text)
{
    printf("%s",text);
}

void platform::outputToLogFile(const char* text)
{

}
#endif
