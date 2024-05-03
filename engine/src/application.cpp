#include "application.h"

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

const char* mercury::Application::GetName()
{
	return "Please Spcify mercury::Application child class";
}

void ApplicationRun()
{
	gApplication->Initialize();
	while (gApplication->Update())
	{

	}
	gApplication->Shutdown();
}
