#include "engine.h"
#include "platform.h"
#include "mercury_log.h"
#include "llri.h"
#include "mercury_imgui.h"
#include "swapchain.h"

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

void renderMainOutput()
{
	static void* nativeWindowHandle = nullptr;

	if (nativeWindowHandle != platform::getMainWindowHandle())
	{
		nativeWindowHandle = platform::getMainWindowHandle();

		if (nativeWindowHandle == nullptr)
		{
			llri::swapchain::destroy();

		}
		else
		{
			llri::swapchain::create(nativeWindowHandle);
		}
	}

	if (nativeWindowHandle != nullptr)
	{
		llri::swapchain::update();
	}
}

bool engine::update()
{	
	platform::update();
	imgui::update();

	renderMainOutput();

	return true;
}

void engine::renderCallback(llri::context ctx)
{
	imgui::render(ctx);
}