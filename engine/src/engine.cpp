#pragma warning( disable : 4668)
#pragma warning( disable : 4710)
#pragma warning( disable : 4711)

#include <vector>

import mercury;

std::vector<mercury::Application*> all_applications;

mercury::Application::Application()
{
	all_applications.push_back(this);
}

mercury::Application::~Application()
{

}

bool MercuryRun()
{
	for (auto* app : all_applications)
	{
		app->Initialize();

		while (app->Update())
		{

		}

		app->Shutdown();
	}

	return true;
}