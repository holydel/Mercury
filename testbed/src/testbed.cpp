import mercury;
import mercury.system;

class TestBedApplication : public mercury::Application
{
public:
	TestBedApplication();
	virtual ~TestBedApplication() override;

	virtual bool Initialize() override;

	virtual bool Shutdown() override;

	virtual bool Update() override;

	virtual const char* GetName() override
	{
		return "TestBed Application";
	}
};

TestBedApplication::TestBedApplication()
{

}

TestBedApplication::~TestBedApplication()
{

}

bool TestBedApplication::Initialize()
{
	mercury::system::DebugOutput("foo:%d\n",100);
	return true;
}

bool TestBedApplication::Shutdown()
{
	return true;
}

bool TestBedApplication::Update()
{
	static int a = 0;
	a++;

	return a < 5;
}

static TestBedApplication myApp;

