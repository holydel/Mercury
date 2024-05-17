#include "application.h"
#include "platform.h"

mercury::Application* gApplication = nullptr;

mercury::Application::Application()
{
	gApplication = this;
}

mercury::Application::~Application()
{
	gApplication = nullptr;
}

bool mercury::Application::Initialize()
{
	return true;
}

bool mercury::Application::Shutdown()
{
	return true;
}

bool mercury::Application::Update()
{
	return true;
}

void ApplicationRun()
{
	platformInitialize();

#ifdef MERCURY_DESKTOP
	platformCreateMainWindow();
#endif

	gApplication->Initialize();
	while (gApplication->Update())
	{
		platformUpdate();
	}
	gApplication->Shutdown();

#ifdef MERCURY_DESKTOP
	platformDestroyMainWindow();
#endif

	platformShutdown();
}
