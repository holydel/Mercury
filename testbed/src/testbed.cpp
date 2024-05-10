#include "mercury.h"

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


#include "mercury.h"


#include <stdio.h>
#include <android/native_activity.h>

JNIEXPORT void ANativeActivity_onCreate2(ANativeActivity* activity,
	void* savedState, size_t savedStateSize)
{
	const char* info = mercury::platform::GetInfo();
	printf("bbb");
}