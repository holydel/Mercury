#pragma once

#include "mercury_api.h"

namespace llvri
{
	bool initialize();
	bool createSession();

	void shutdown();

	bool update();
}