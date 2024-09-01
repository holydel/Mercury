#include "mercury_entrypoint.h"
#include <cmath>
#include <chrono>
#include <mercury.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <thread>
#include <Windows.h>

using namespace mercury;

class TestBedApplication : public mercury::Application
{
	bool isRunning = true;
	mercury::Material mat;
public:
	TestBedApplication();
	virtual ~TestBedApplication() override;

	virtual bool Initialize() override;

	virtual bool Shutdown() override;

	virtual bool Update() override;
	virtual bool Render() override;
	virtual bool PreRender() override;

	virtual void OnApplicationClose() override;
};

TestBedApplication::TestBedApplication()
{
	config.appName = "TestBed Application";	
	config.render.adapterID = 0;
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

bool TestBedApplication::PreRender()
{
	if (!mat.isValid())
	{
		
		mercury::Material::Desc desc;
		desc.vertexShader = mercury::internal_shader::TestTriangleVert();
		desc.fragmentShader = mercury::internal_shader::TestTriangleFrag();

		mat = mercury::Material::Create(desc);
		
	}
	return true;
}

bool TestBedApplication::Render()
{
	if (mat.isValid())
	{
		mercury::rendering::SetPSO(mat);
		mercury::rendering::Draw(3);
	}
	return true;
}

bool TestBedApplication::Update()
{
	static auto time = std::chrono::steady_clock::now();
	static int frames = 0;
	frames++;
	if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count() >= 1000) {
		time = std::chrono::steady_clock::now();
		mercury::write_log_message("FPS:%d",frames);
		frames = 0;
	}
	//mercury::rendering::SetBackGroundColor(0.5f, sin(value) * 0.5f + 0.5f, 0.0f, 1.0f);
	return isRunning;
}

void TestBedApplication::OnApplicationClose()
{
	isRunning = false;
}

//static TestBedApplication myApp;