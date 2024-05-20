#include "../llri.h"
#include "mercury_log.h"

bool llri::initialize()
{
	mercury::log("LowLevel Rendering Interface Initialize: VULKAN");
	return true;
}

void llri::shutdown()
{
	mercury::log("LowLevel Rendering Interface Shutdown: VULKAN");
}
