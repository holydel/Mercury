#pragma once
#include "mercury_api.h"
#include <cstddef>

void platformInitialize();
void platformShutdown();
void platformUpdate();

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
void platformCreateMainWindow();
void platformDestroyMainWindow();
#endif