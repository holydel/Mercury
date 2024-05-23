#pragma once

#include "mercury_api.h"

namespace llri
{
	bool initialize();
	void shutdown();

	bool update();

	struct CommandList
	{
		void* impl;		
	};

	CommandList BeginOneTimeSubmitCommandList();
	void EndOneTimeSubmitCommandList(CommandList clist);
}