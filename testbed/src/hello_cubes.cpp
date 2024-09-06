#include "mercury_entrypoint.h"
#include <cmath>
#include <chrono>
#include <mercury.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <thread>

using namespace mercury;

class HelloCubesApplication : public mercury::Application
{
	bool isRunning = true;
	mercury::Material mat;
	mercury::Material mat2;

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

	if (!mat2.isValid())
	{
		mercury::Material::Desc desc;
		desc.vertexShader = mercury::internal_shader::StaticDedicatedMeshVS();
		desc.fragmentShader = mercury::internal_shader::StaticDedicatedMeshPS();
		desc.shaderInputs.numPushConstants = 16;
		desc.topology = Topology::TriangleList;
		desc.vertexInput.AddAttrib(Format::R32G32B32_SFLOAT, 0, 0, 0,"POSITION");
		desc.vertexInput.AddAttrib(Format::R32G32B32_SFLOAT, 12, 1, 0, "NORMAL");
		desc.vertexInput.AddAttrib(Format::R32G32_SFLOAT, 24, 2, 0, "TEXCOORD");
		desc.vertexInput.AddAttrib(Format::R32G32B32A32_SFLOAT, 32, 3, 0, "COLOR");
		mat2 = mercury::Material::Create(desc);
	}

	return true;
}
static float angle = 0.0f;

bool HelloCubesApplication::Render()
{
	if (mat2.isValid())
	{
		float cameraX = 4.0f * cos(angle);
		float cameraZ = 4.0f * sin(angle);
		angle += 0.01f;

		// Define your model, view, and projection matrices
		glm::mat4 model = glm::mat4(1.0f); // Identity matrix for the model
		glm::mat4 view = glm::lookAt(glm::vec3(cameraX, 0.0f, cameraZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f); // Adjust aspectRatio as needed

		// Calculate the Model-View-Projection matrix
		glm::mat4 mvp = projection * view * model;

		canvas::DrawDedicatedStaticMesh(mat2, cubeMesh, mvp);
	}

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