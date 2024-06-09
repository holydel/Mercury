#pragma once

class SBProject
{
	static SBProject* CreateNew(const char* name, const char* path);
public:
	static void Create();
	static void Open(const char* path);

	void Save();
	void SaveAs(const char* path);
	void Export(const char* path);
	void CreatePAK();

	static void Draw();

	static SBProject* gCurrent;

	void Load();
	void UnLoad();
};

 