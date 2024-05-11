#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_WINDOWS

#include "../application.h"

#include <Windows.h>
//console main
int main()
{
    ApplicationRun();
    return 0;
}

//desktop main
int WINAPI WinMain([[maybe_unused]] HINSTANCE hInstance,    // HANDLE TO AN INSTANCE.  This is the "handle" to YOUR PROGRAM ITSELF.
    [[maybe_unused]] HINSTANCE hPrevInstance,// USELESS on modern windows (totally ignore hPrevInstance)
    [[maybe_unused]] LPSTR szCmdLine,        // Command line arguments.  similar to argv in standard C programs
    [[maybe_unused]] int iCmdShow)          // Start window maximized, minimized, etc.
{
    ApplicationRun();
    return 0;
}


#endif
