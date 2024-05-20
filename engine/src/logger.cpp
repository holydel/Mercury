#include "mercury_log.h"
#include <cstdarg>
#include <format>
#include <string>

void mercury::log(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    printf("\n");
    va_end(argptr);
}