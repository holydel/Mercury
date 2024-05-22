#include "mercury_log.h"
#include <stdarg.h>
#include <string>
#include "platform.h"

void mercury::write_log_message(const char* format, ...)
{
    char buff[1024];

    va_list argptr;
    va_start(argptr, format);
    int pos = std::vsnprintf(buff, 1024, format, argptr);
    buff[pos] = '\n';
    buff[pos+1] = 0;
    va_end(argptr);

    platform::outputToDebugConsole(buff);
}