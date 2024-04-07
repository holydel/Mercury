module;

export module mercury;

import std.core;

export namespace mercury
{
	export class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual bool Initialize()
		{
			return true;
		}

		virtual bool Shutdown()
		{
			return true;
		}

		virtual bool Update()
		{
			return true;
		}

		virtual std::string_view GetName()
		{
			return "NoName Mercury Project";
		}
	};

	std::vector<Application*> all_applications;

	Application::Application()
	{
		all_applications.push_back(this);		
	}

	Application::~Application()
	{

	}

	export bool Run()
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
}