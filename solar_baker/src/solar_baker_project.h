#pragma once
#include <string>
#include <filesystem>
#include <pugixml.hpp>
#include "sbproj_source.h"

class SBProject
{
	static SBProject* CreateNew(const char* name, const char* path);

	pugi::xml_document doc;
	std::string name;
	std::filesystem::path path;

	SBProjSource* rootSource = nullptr;

public:
	SBProject();
	~SBProject();

	static void Create();
	static void Open(const char* path);

	void Save();
	void SaveAs(const char* path);
	void Export(const char* path);
	void CreatePAK();

	static void Draw();

	static SBProject* gCurrent;

	void Load(const char* path);
	void UnLoad();

	void ReloadSources();
	void DrawProjectSources();
};

 