#include "mercury_entrypoint.h"
#include <cmath>
#include <chrono>
#include <mercury.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <thread>
#include <Windows.h>

using namespace mercury;

class HelloCubesApplication : public mercury::Application
{
	bool isRunning = true;
	mercury::Material mat;

	struct Rectangle
	{
		float x, y, width, height, dx, dy;
		glm::vec4 color;
	};

	std::vector< Rectangle> allRectangles;
	Mesh cubeMesh;

public:
	HelloCubesApplication();
	virtual ~HelloCubesApplication() override;

	virtual bool Initialize() override;

	virtual bool Shutdown() override;

	virtual bool Update() override;
	virtual bool Render() override;
	virtual bool PreRender() override;

	virtual void OnApplicationClose() override;
};

HelloCubesApplication::HelloCubesApplication()
{
	config.appName = "HelloCubesApplication";
	config.render.adapterID = 0;
}

HelloCubesApplication::~HelloCubesApplication()
{

}

bool HelloCubesApplication::Initialize()
{
	int numRectangles = 100;

	for (int i = 0; i < numRectangles; ++i)
	{
		Rectangle& r = allRectangles.emplace_back();
		r.x = rand() % 1280;
		r.y = rand() % 720;
		r.width = rand() % 8 + 2;
		r.height = rand() % 8 + 2;

		r.dx = float((rand() % 1024) - 512) * 0.01f;
		r.dy = float((rand() % 1024) - 512) * 0.01f;

		r.color.x = float(rand() % 255) / 255.0f;
		r.color.y = float(rand() % 255) / 255.0f;
		r.color.z = float(rand() % 255) / 255.0f;
		r.color.w = float(rand() % 255) / 255.0f;
	}

	cubeMesh = parametrical_meshes::CreateCube(1.0f);
	return true;
}

bool HelloCubesApplication::Shutdown()
{
	return true;
}

bool HelloCubesApplication::PreRender()
{
	if (!mat.isValid())
	{

		mercury::Material::Desc desc;
		desc.vertexShader = mercury::internal_shader::ColoredSpriteVS();
		desc.fragmentShader = mercury::internal_shader::ColoredSpritePS();
		desc.shaderInputs.numPushConstants = 8;
		desc.topology = Topology::TriangleStrip;

		mat = mercury::Material::Create(desc);

	}
	return true;
}

bool HelloCubesApplication::Render()
{
	if (mat.isValid())
	{
		for (auto& r : allRectangles)
		{
			canvas::DrawDedicatedRect(mat, r.x, r.y, r.width, r.height, r.color);
		}
	}
	return true;
}

bool HelloCubesApplication::Update()
{
	for (auto& r : allRectangles)
	{
		r.x += r.dx;
		r.y += r.dy;

		if (r.x < 0)
			r.dx = abs(r.dx);
		if (r.y < 0)
			r.dy = abs(r.dy);

		if (r.x + r.width > 1280)
			r.dx = -abs(r.dx);
		if (r.y + r.height > 720)
			r.dy = -abs(r.dy);
	}

	static auto time = std::chrono::steady_clock::now();
	static int frames = 0;
	frames++;
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count() >= 1000) {
		time = std::chrono::steady_clock::now();
		mercury::write_log_message("FPS:%d", frames);
		frames = 0;
	}
	//mercury::rendering::SetBackGroundColor(0.5f, sin(value) * 0.5f + 0.5f, 0.0f, 1.0f);
	return isRunning;
}

void HelloCubesApplication::OnApplicationClose()
{
	isRunning = false;
}

static HelloCubesApplication myApp;