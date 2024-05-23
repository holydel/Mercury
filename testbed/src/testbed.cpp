#include "mercury_entrypoint.h"
#include <cmath>
#include <chrono>
#include <mercury_log.h>

class TestBedApplication : public mercury::Application
{
	bool isRunning = true;
	float value = 0.0f;
	std::chrono::time_point<std::chrono::steady_clock,std::chrono::milliseconds> timer;
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
	static auto time = std::chrono::steady_clock::now();
	static int frames = 0;
	frames++;
	if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count() >= 1000) {
		time = std::chrono::steady_clock::now();
		mercury::write_log_message("FPS:%d",frames);
		frames = 0;
	}
	mercury::rendering::SetBackGroundColor(0.5f, sin(value) * 0.5f + 0.5f, 0.0f, 1.0f);
	return isRunning;
}

void TestBedApplication::OnApplicationClose()
{
	isRunning = false;
}

static TestBedApplication myApp;