#pragma once

#include "mercury_api.h"

namespace llri
{
	struct context
	{
		void* impl;

		void Flush();
		void Finish();
	};
}