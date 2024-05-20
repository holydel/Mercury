#include "application.h"
#include "platform.h"
#include "engine.h"

using namespace mercury;

Application* gApplication = nullptr;

Application::Application()
{
	gApplication = this;
}

Application::~Application()
{
	gApplication = nullptr;
}

bool Application::Initialize()
{
	return true;
}

bool Application::Shutdown()
{
	return true;
}

bool Application::Update()
{
	return true;
}

void Application::OnApplicationClose()
{

}

void ApplicationRun()
{
	engine::initialize();

#ifdef MERCURY_DESKTOP
	platform::createMainWindow();
#endif

	gApplication->Initialize();
	while (gApplication->Update())
	{
		engine::update();
	}
	gApplication->Shutdown();

#ifdef MERCURY_DESKTOP
	platform::destroyMainWindow();
#endif

	engine::shutdown();
}
