#include "mercury_entrypoint.h"
#include <cmath>

class TestBedApplication : public mercury::Application
{
	bool isRunning = true;
	float value = 0.0f;

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
	value += 0.1f;

	mercury::rendering::SetBackGroundColor(0.5f, sin(value) * 0.5f + 0.5f, 0.0f, 1.0f);
	return isRunning;
}

void TestBedApplication::OnApplicationClose()
{
	isRunning = false;
}

static TestBedApplication myApp;