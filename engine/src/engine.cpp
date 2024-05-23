#include "engine.h"
#include "platform.h"
#include "mercury_log.h"
#include "llri.h"
#include "mercury_imgui.h"

using namespace mercury;

bool engine::initialize()
{
	mercury::write_log_message("engine initialize");
	platform::initialize();
	llri::initialize();
	return true;
}

void engine::shutdown()
{	
	llri::shutdown();
	platform::shutdown();
	mercury::write_log_message("engine shutdown");
}

bool engine::update()
{
	platform::update();
	imgui::update();
	llri::update();
	
	return true;
}