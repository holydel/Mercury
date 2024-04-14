#pragma warning( disable : 4668)
#pragma warning( disable : 5039)
#pragma warning( disable : 4710)
#pragma warning( disable : 4711)

#include <stdio.h>
#include <Windows.h>

import mercury;
import mercury.system;

extern bool MercuryRun();

//console main
int main()
{
    MercuryRun();
	return 0;
}

//desktop main
int WINAPI WinMain([[maybe_unused]] HINSTANCE hInstance,    // HANDLE TO AN INSTANCE.  This is the "handle" to YOUR PROGRAM ITSELF.
    [[maybe_unused]] HINSTANCE hPrevInstance,// USELESS on modern windows (totally ignore hPrevInstance)
    [[maybe_unused]] LPSTR szCmdLine,        // Command line arguments.  similar to argv in standard C programs
    [[maybe_unused]] int iCmdShow)          // Start window maximized, minimized, etc.
{
    MercuryRun();
    return 0;
}


void mercury::system::DebugOutput(const char* utf8string, ...)
{
    constexpr int MAX_BUFF_SIZE = 256;

    char buff[MAX_BUFF_SIZE + 2];

    va_list argptr;
    va_start(argptr, utf8string);
    int writtenChars = vsprintf_s(buff, utf8string, argptr);  
    if (writtenChars > 0)
    {
        if (buff[writtenChars - 1] != '\n')
        {
            buff[writtenChars] = '\n';
            buff[writtenChars+1] = 0;
        }
    }    
    va_end(argptr);

    OutputDebugStringA(buff);
}
