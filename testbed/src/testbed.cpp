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
	float value = 0.0f;
	std::chrono::time_point<std::chrono::steady_clock,std::chrono::milliseconds> timer;
	mercury::Material mat;
	mercury::Material mat2;
	mercury::Buffer pointCache;

	mercury::Buffer pointCacheGPUCompressed;
	mercury::Buffer pointCacheGPU;
	
		struct Particle
	{
		float x, y, z;
		float r, g, b;
	};

	struct ParticlePacked
	{
		short hx, hy, hz;
		short color_packed;
	};

	static constexpr int max_particles = 4'096'000;
	static constexpr int max_pages = max_particles * sizeof(Particle) / 65536;



	FILE* pcache_file = nullptr;
	FILE* pcache_file_meta = nullptr;

	int numFrames = 0;
	int frame = 0;
	int pages[max_pages];
	rendering::DecompressPage decompPages[max_pages];
	int particlesNum = 0;
	int particlesNum2 = 0;
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
	if (!mat2)
	{
		//{
		//	mercury::Material::Desc desc;
		//	desc.vertexShader = mercury::internal_shader::TestTriangleVert();
		//	desc.fragmentShader = mercury::internal_shader::TestTriangleFrag();

		//	mat = mercury::Material::Create(desc);
		//}

		{
			mercury::Material::Desc desc;
			desc.vertexShader = mercury::internal_shader::TestPointCacheVert();
			desc.fragmentShader = mercury::internal_shader::TestPointCacheFrag();

			desc.vertexInput.AddAttrib(mercury::Format::R32G32B32_SFLOAT, 0, 0, 0);
			desc.vertexInput.AddAttrib(mercury::Format::R32G32B32_SFLOAT, 12, 1, 0);

			desc.renderState.blendMode = mercury::BlendMode::Add;

			mat2 = mercury::Material::Create(desc);
		}

		int meshSize = sizeof(Particle) * max_particles;


		pointCache = mercury::Buffer::Create(meshSize, Buffer::HeapType::UPLOAD);

		pointCacheGPUCompressed = mercury::Buffer::Create(meshSize, Buffer::HeapType::DEFAULT);
		pointCacheGPU = mercury::Buffer::Create(meshSize, Buffer::HeapType::DEFAULT);

		fopen_s(&pcache_file, "D:\\temp\\pointcachef32.gdeflate", "rb");
		fopen_s(&pcache_file_meta, "D:\\temp\\pointcachef32.gdeflate.meta", "rb");

		fread(&numFrames, 4, 1, pcache_file_meta);
	}

	fread(&particlesNum, 4, 1, pcache_file_meta);
	int pagesNum = 0;
	fread(&pagesNum, 4, 1, pcache_file_meta);
	fread(pages, 4, pagesNum, pcache_file_meta);

	int srcOffset = 0;
	int dstOffset = 0;

	for (int i = 0; i < pagesNum; ++i)
	{
		assert(pages[i] % 4 == 0);
		assert(srcOffset % 4 == 0);

		decompPages[i].srcSize = pages[i];
		decompPages[i].srcOffset = srcOffset;
		decompPages[i].dstSize = 0;
		decompPages[i].dstOffset = dstOffset;


		srcOffset += pages[i];
		dstOffset += 65536;
	}

	fread(pointCache.MappedData(), srcOffset, 1, pcache_file);

	static int decompress = 150;

	if (decompress == 140)
	{
		mercury::rendering::CopyBuffer(pointCache,pointCacheGPUCompressed, srcOffset);

		mercury::rendering::DecompressBuffer(pointCacheGPUCompressed, pointCacheGPU, decompPages, pagesNum);
		particlesNum2 = particlesNum;
	}
	

	decompress--;
	return true;
}

bool TestBedApplication::Render()
{

	
	//mercury::rendering::SetPSO(mat);
	//mercury::rendering::Draw(3);

	struct pcdata
	{
		glm::mat4 mvp;
		glm::vec3 EyePos;
		float time;
	} pc;

	pc.EyePos.x = 0;
	pc.EyePos.y = 6;
	pc.EyePos.z = -4;

	auto view = glm::lookAtLH(pc.EyePos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	auto proj = glm::perspectiveFovLH_ZO(1.4f, 1280.0f, 720.0f, 0.1f, 5000.0f);


	pc.mvp = proj * view;

	mercury::rendering::SetPSO(mat2);

	mercury::rendering::SetBuffer(pointCacheGPU);

	mercury::rendering::SetConstantsData(&pc, sizeof(pc));

	if (particlesNum2 > 0)
	{
		mercury::rendering::Draw(particlesNum2); // particlesNum
	}
	

	frame++;

	if (frame >= numFrames-1)
	{
		frame = 0;
		fseek(pcache_file, 0, SEEK_SET);
		fseek(pcache_file_meta, 4, SEEK_SET);
	}
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
	//mercury::rendering::SetBackGroundColor(0.5f, sin(value) * 0.5f + 0.5f, 0.0f, 1.0f);
	return isRunning;
}

void TestBedApplication::OnApplicationClose()
{
	isRunning = false;
}

static TestBedApplication myApp;