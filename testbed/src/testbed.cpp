#include "mercury_entrypoint.h"

class TestBedApplication : public mercury::Application
{
	bool isRunning = true;

public:
	TestBedApplication();
	virtual ~TestBedApplication() override;

	virtual bool Initialize() override;

	virtual bool Shutdown() override;

	virtual bool Update() override;

	virtual void OnApplicationClose() override;
};

TestBedApplication::TestBedApplication()
{
	config.appName = "TestBed Application";	
}

TestBedApplication::~TestBedApplication()
{

}

bool TestBedApplication::Initialize()
{
	return true;
}

bool TestBedApplication::Shutdown()
{
	return true;
}

bool TestBedApplication::Update()
{
	return isRunning;
}

void TestBedApplication::OnApplicationClose()
{
	isRunning = false;
}

static TestBedApplication myApp;