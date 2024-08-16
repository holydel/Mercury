#include "mercury_entrypoint.h"
#include <cmath>
#include <chrono>
#include <mercury_log.h>

#include "screen_main.h"
#include "shader_compiler.h"
#include "solar_baker_project.h"

bool gIsRunning = true;

class SolarBaker : public mercury::Application
{

	float value = 0.0f;
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> timer;
	MainScreen mainScreen;

public:
	SolarBaker();
	virtual ~SolarBaker() override;

	virtual bool Initialize() override;

	virtual bool Shutdown() override;

	virtual bool Update() override;

	virtual void OnApplicationClose() override;
};

SolarBaker::SolarBaker()
{
	config.appName = "Solar Baker";

	config.output.width = 1840;
	config.output.height = 864;
}

SolarBaker::~SolarBaker()
{

}

bool SolarBaker::Initialize()
{
	ShaderCompiler::initialize();

	mercury::rendering::SetBackGroundColor(0.1f, 0.15f, 0.2f, 1.0f);
	
	mercury::imgui_interface::AddScreen(&mainScreen);

	//TODO: from recent
	SBProject::Open(u8"C:\\Users\\holyd\\Documents\\NoName Mercury Project");
	return true;
}

bool SolarBaker::Shutdown()
{
	ShaderCompiler::shutdown();

	return true;
}

bool SolarBaker::Update()
{
	return gIsRunning;
}

void SolarBaker::OnApplicationClose()
{
	gIsRunning = false;
}

static SolarBaker myApp;

void SetMainWindowTitle(const char* u8string)
{
	myApp.SetWindowTitle(u8string);
}