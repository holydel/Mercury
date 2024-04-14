module;

export module mercury;

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

		virtual const char* GetName()
		{
			return "NoName Mercury Project";
		}
	};
}