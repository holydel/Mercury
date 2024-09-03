#include "mercury_entrypoint.h"
#include <cmath>
#include <chrono>
#include <mercury_log.h>
#include "material_preview.h"

#include "shader_compiler.h"
#include "screen_main.h"
#include "solar_baker_project.h"
#include "imgui_fonts.h"

bool gIsRunning = true;
using namespace mercury;

ImFont* gMainFont = nullptr;

class SolarBaker : public mercury::Application
{

	float value = 0.0f;
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> timer;
	MainScreen mainScreen;
	bool isInitialized = false;

public:
	SolarBaker();
	virtual ~SolarBaker() override;

	virtual bool Initialize() override;

	virtual bool Shutdown() override;

	virtual bool Update() override;

	virtual bool Render() override;

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

bool SolarBaker::Render()
{
	if (!isInitialized)
	{
		auto& io = ImGui::GetIO();

		gMainFont = io.Fonts->AddFontFromFileTTF("assets\\fonts\\FiraCode-SemiBold.ttf", 20);
		
		io.Fonts->Build();
		isInitialized = true;
	}
	MaterialPreview::Instance().Draw();



	return false;
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


ImFont* SolarBakerFonts::MainFont()
{
	return gMainFont;
}
