#include "mercury_entrypoint.h"

class TestBedApplication : public mercury::Application
{
public:
	TestBedApplication();
	virtual ~TestBedApplication() override;

	virtual bool Initialize() override;

	virtual bool Shutdown() override;

	virtual bool Update() override;
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
	return true;
}

static TestBedApplication myApp;