#pragma once
#include "mercury_api.h"

namespace mercury
{
	void write_log_message(const char* mask, ...);
	void write_log_message(const wchar_t* mask, ...);
}