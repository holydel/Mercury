#include "engine.h"
#include "platform.h"
#include "mercury_log.h"
#include "llri.h"

using namespace mercury;

bool engine::initialize()
{
	mercury::log("engine initialize");
	platform::initialize();
	llri::initialize();
	return true;
}

void engine::shutdown()
{	
	llri::shutdown();
	platform::shutdown();
	mercury::log("engine shutdown");
}

bool engine::update()
{
	platform::update();
	return true;
}