#include "screen_main.h"
#include "imgui.h"
#include "solar_baker_project.h"
#include <filesystem>
#include "libdeflate.h"

SBProject* gCurrentProject = nullptr;
MainScreen* MainScreen::gMainScreenInstance = nullptr;

void MainScreen::InitializeRecentlyProjects(mercury::Application* app)
{
	recentlyProjects = app->GetRuntimeSettingsList("RecentlyProjects");
}

void MainScreen::SaveRecentlyProjects(mercury::Application* app)
{
	if (!recentlyProjects.empty())
	{
		app->SetRuntimeSettingsList("RecentlyProjects", recentlyProjects);
	}
}

void MainScreen::Draw()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New..."))
		{
			SBProject::Create();
		}

		if (ImGui::MenuItem(u8"Open...")) {}

		if (ImGui::BeginMenu("Open Recent", false))
		{
			for (auto proj : recentlyProjects)
			{
				if (ImGui::MenuItem(proj.c_str()))
				{

				}
			}
			ImGui::EndMenu();
		}

		bool selected = false;
		if (ImGui::MenuItem("Save", "Ctrl+S", &selected))
		{
			if (SBProject::gCurrent)
			{
				SBProject::gCurrent->Save();
			}
		}

		if (ImGui::MenuItem("Save As...")) {}

		ImGui::Separator();

		if (ImGui::MenuItem("Exit", "Alt+F4")) {
			extern bool gIsRunning;
			gIsRunning = false;
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
		if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
		ImGui::Separator();
		if (ImGui::MenuItem("Cut", "CTRL+X")) {}
		if (ImGui::MenuItem("Copy", "CTRL+C")) {}
		if (ImGui::MenuItem("Paste", "CTRL+V")) {}
		
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Assets"))
	{
		if (ImGui::MenuItem("Shaders")) {
			mercury::imgui_interface::AddScreen(&shadersScreen);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("DebugConvert Blender Particle Cache")) {
			DebugConvertBlenderPhysicsCache();
		}

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	SBProject::Draw();
}

typedef unsigned short ushort;
typedef unsigned int uint;

uint as_uint(const float x) {
	return *(uint*)&x;
}
float as_float(const uint x) {
	return *(float*)&x;
}

float half_to_float(const ushort x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint e = (x & 0x7C00) >> 10; // exponent
	const uint m = (x & 0x03FF) << 13; // mantissa
	const uint v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
	return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
}
ushort float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
	const uint e = (b & 0x7F800000) >> 23; // exponent
	const uint m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
	return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}


void MainScreen::DebugConvertBlenderPhysicsCache()
{
	struct ParticleOut
	{
		float x, y, z;
		float r, g, b;
	};

	struct ParticleOutPacked
	{
		unsigned short x, y, z;
		unsigned short color_r5g6b5;
	};

	struct ParticleIn
	{
		int index;
		float x, y, z;
		float vx, vy, vz;
	};

	struct ParticleFrame
	{
		int num_particles = 0;
		ParticleIn* particles_data = nullptr;
	};

	struct BPhysHeader
	{
		char header_magic[8];
		int32_t obj_type;
		int32_t count;
		int32_t datatype_flags;
	};

	namespace fs = std::filesystem;

	fs::path bphys_path = u8"C:\\Users\\holyd\\Documents\\TestPSystem\\blendcache_untitled";

	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float minZ = FLT_MAX;

	float maxX = FLT_MIN;
	float maxY = FLT_MIN;
	float maxZ = FLT_MIN;

	float minVX = FLT_MAX;
	float minVY = FLT_MAX;
	float minVZ = FLT_MAX;

	float maxVX = FLT_MIN;
	float maxVY = FLT_MIN;
	float maxVZ = FLT_MIN;

	std::vector< ParticleFrame> allFrames;
	allFrames.reserve(256);

	for (const auto& entry : fs::directory_iterator(bphys_path)) {
		
		if (entry.is_regular_file())
		{
			auto ext = entry.path().extension().u8string();
			if (ext == ".bphys")
			{
				FILE* f = nullptr;
				fopen_s(&f, entry.path().u8string().c_str(), "rb");
				
				BPhysHeader header;
				fread(&header, sizeof(header), 1, f);

				auto& pframe = allFrames.emplace_back();
				pframe.num_particles = header.count;
				pframe.particles_data = new ParticleIn[pframe.num_particles];

				fread(pframe.particles_data, sizeof(ParticleIn), pframe.num_particles, f);



				for (int i = 0; i < pframe.num_particles; ++i)
				{
					auto& p = pframe.particles_data[i];

					maxX = std::max(maxX, p.x);
					maxY = std::max(maxY, p.y);
					maxZ = std::max(maxZ, p.z);
					minX = std::min(minX, p.x);
					minY = std::min(minY, p.y);
					minZ = std::min(minZ, p.z);

					maxVX = std::max(maxVX, p.vx);
					maxVY = std::max(maxVY, p.vy);
					maxVZ = std::max(maxVZ, p.vz);
					minVX = std::min(minVX, p.vx);
					minVY = std::min(minVY, p.vy);
					minVZ = std::min(minVZ, p.vz);
				}

				fclose(f);
			}
		}
		
	}

	int countFrames = allFrames.size();

	FILE* outFile = nullptr;
	fopen_s(&outFile, "D:\\temp\\pointcachef32.raw", "wb");

	FILE* outFile2 = nullptr;
	fopen_s(&outFile2, "D:\\temp\\pointcache_packed.raw", "wb");


	fwrite(&countFrames, 4, 1, outFile);
	fwrite(&countFrames, 4, 1, outFile2);

	std::vector<std::vector<ParticleOut>*> particles;
	std::vector<std::vector<ParticleOutPacked>*> particles_packed;

	particles.reserve(countFrames);
	particles_packed.reserve(countFrames);

	for (auto& f : allFrames)
	{
		fwrite(&f.num_particles, 4, 1, outFile);
		fwrite(&f.num_particles, 4, 1, outFile2);

		auto& particlesVec = particles.emplace_back(new std::vector<ParticleOut>(f.num_particles));
		auto& particlesPackedVec = particles_packed.emplace_back(new std::vector<ParticleOutPacked>(f.num_particles));

		for (int i = 0; i < f.num_particles; ++i)
		{
			auto& p = f.particles_data[i];

			auto& pout = (*particlesVec)[i];

			pout.x = p.x;
			pout.y = p.y;
			pout.z = p.z;
			
			pout.r = (p.vx - minVX) / (maxVX - minVX);
			pout.g = (p.vy - minVY) / (maxVY - minVY);
			pout.b = (p.vz - minVZ) / (maxVZ - minVZ);

			fwrite(&pout, sizeof(ParticleOut), 1, outFile);

			auto& pp = (*particlesPackedVec)[i];

			pp.x = float_to_half(p.x);
			pp.y = float_to_half(p.y);
			pp.z = float_to_half(p.z);
			
			unsigned short r = pout.r * 32;
			unsigned short g = pout.g * 64;
			unsigned short b = pout.b * 32;

			pp.color_r5g6b5 = r << 11 | g << 5 | b;

			fwrite(&pp, sizeof(ParticleOutPacked), 1, outFile2);
		}

		delete[] f.particles_data;
	}

	fclose(outFile);
	fclose(outFile2);


	FILE* outFileComp = nullptr;
	fopen_s(&outFileComp, "D:\\temp\\pointcachef32.gdeflate", "wb");

	FILE* outFile2Comp = nullptr;
	fopen_s(&outFile2Comp, "D:\\temp\\pointcache_packed.gdeflate", "wb");

	FILE* outFileCompM = nullptr;
	fopen_s(&outFileCompM, "D:\\temp\\pointcachef32.gdeflate.meta", "wb");

	FILE* outFile2CompM = nullptr;
	fopen_s(&outFile2CompM, "D:\\temp\\pointcache_packed.gdeflate.meta", "wb");

	auto compressor = libdeflate_alloc_gdeflate_compressor(12);

	std::vector<std::vector< libdeflate_gdeflate_out_page>> statndard_pages(particles.size());
	std::vector<std::vector< libdeflate_gdeflate_out_page>> packed_pages(particles.size());

	fwrite(&countFrames, 4, 1, outFileCompM);
	fwrite(&countFrames, 4, 1, outFile2CompM);

	size_t compressedSizeTotal = 0;
	size_t uncompressedSizeTotal = 0;

	for (int i = 0; i < particles.size(); ++i)
	{
		int numParticles = particles[i]->size();
		int uncompressedDataSize = numParticles * sizeof(ParticleOut);
		uncompressedSizeTotal += uncompressedDataSize;

		fwrite(&numParticles, 4, 1, outFileCompM);
		if (numParticles > 0)
		{
			size_t npages = 0;
			size_t maxCompressedPageSize = libdeflate_gdeflate_compress_bound(compressor, uncompressedDataSize, &npages);

			statndard_pages[i].resize(npages);
			for (int j = 0; j < npages; ++j)
			{
				statndard_pages[i][j].data = malloc(maxCompressedPageSize);
				statndard_pages[i][j].nbytes = maxCompressedPageSize;
			}

			int numPages = npages;

			fwrite(&numPages, 4, 1, outFileCompM);

			size_t compressedBytesAllPages = libdeflate_gdeflate_compress(compressor, particles[i]->data(), uncompressedDataSize, statndard_pages[i].data(), npages);

			for (int j = 0; j < npages; ++j)
			{
				int numCompressedBytes = statndard_pages[i][j].nbytes;
				fwrite(&numCompressedBytes, 4, 1, outFileCompM);

				compressedSizeTotal += numCompressedBytes;
				fwrite(statndard_pages[i][j].data, numCompressedBytes, 1, outFileComp);
				free(statndard_pages[i][j].data);

				compressedBytesAllPages -= numCompressedBytes;
			}

			assert(compressedBytesAllPages == 0);
		}

		delete particles[i];
	}

	fclose(outFileCompM);
	fclose(outFileComp);

	for (int i = 0; i < particles_packed.size(); ++i)
	{
		int numParticles = particles_packed[i]->size();
		int uncompressedDataSize = numParticles * sizeof(ParticleOutPacked);

		fwrite(&numParticles, 4, 1, outFile2CompM);
		if (numParticles > 0)
		{
			size_t npages = 0;
			size_t maxCompressedPageSize = libdeflate_gdeflate_compress_bound(compressor, uncompressedDataSize, &npages);

			packed_pages[i].resize(npages);
			for (int j = 0; j < npages; ++j)
			{
				packed_pages[i][j].data = malloc(maxCompressedPageSize);
				packed_pages[i][j].nbytes = maxCompressedPageSize;
			}

			int numPages = npages;

			fwrite(&numPages, 4, 1, outFile2CompM);

			size_t compressedBytesAllPages = libdeflate_gdeflate_compress(compressor, particles_packed[i]->data(), uncompressedDataSize, packed_pages[i].data(), npages);

			for (int j = 0; j < npages; ++j)
			{
				int numCompressedBytes = packed_pages[i][j].nbytes;
				compressedBytesAllPages -= numCompressedBytes;

				fwrite(&numCompressedBytes, 4, 1, outFile2CompM);


				fwrite(packed_pages[i][j].data, 1, numCompressedBytes, outFile2Comp);
				free(packed_pages[i][j].data);
			}

			assert(compressedBytesAllPages == 0);
		}

		delete particles_packed[i];
	}

	fclose(outFile2CompM);
	fclose(outFile2Comp);

	libdeflate_free_gdeflate_compressor(compressor);



}