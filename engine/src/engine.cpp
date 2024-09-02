#include "engine.h"
#include "platform.h"
#include "mercury_log.h"
#include "llri.h"
#include "llsi.h"
#include "llvri.h"
#include "mercury_imgui.h"
#include "swapchain.h"
#include "application.h"

using namespace mercury;

bool engine::initialize()
{
	
	mercury::write_log_message("engine initialize");
	platform::initialize();
	
	if (gApplication->config.engine.EnableSound)
		llsi::initialize();

#ifdef MERCURY_USE_XR
	if (gApplication->config.engine.EnableXR)
		llvri::initialize();
#endif
	llri::initialize();

	return true;
}

void engine::shutdown()
{	
	llri::shutdown();
#ifdef MERCURY_USE_XR
	if (gApplication->config.engine.EnableXR)
		llvri::shutdown();
#endif
	if (gApplication->config.engine.EnableSound)
		llsi::shutdown();

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
#ifdef MERCURY_USE_XR
	llvri::update();
#endif
	llsi::update();

	return true;
}

void engine::renderCallback(llri::context ctx)
{
	gApplication->Render();

	imgui::render(ctx);
}

void engine::renderBeforeCallback(llri::context ctx)
{
	gApplication->PreRender();
}

void engine::renderAfterCallback(llri::context ctx)
{
	gApplication->PostRender();
}