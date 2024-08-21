#include "llvri_openxr.h"
#include "mercury_log.h"
#include "mercury_openxr.h"

using namespace mercury;

bool llvri::initialize()
{
	mercury::write_log_message("LowLevel XR Interface Initialize: OPENXR");
	LoadOpenXR_Library();

	return true;
}

void llvri::shutdown()
{

	ShutdownOpenXR_Library();
	mercury::write_log_message("LowLevel XR Interface Shutdown: OPENXR");
}

bool llvri::update()
{

	return true;
}