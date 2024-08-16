#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

struct SBProjSource
{
	SBProjSource* parent = nullptr;
	std::string name;
	bool is_source_file = false;
	std::filesystem::path fullPath;
	std::vector<SBProjSource*> childs;

	void SetParent(SBProjSource* p)
	{
		if (parent != nullptr)
		{
			childs.erase(
				std::remove_if(childs.begin(), childs.end(), [p](SBProjSource* src) {return src == p; }),
				childs.end());
		}

		if (p != nullptr)
		{
			p->childs.push_back(this);
		}
		
		parent = p;
	}
};

struct SBProjShaderSource : public SBProjSource
{
	std::string cachedSource;
	std::vector<uint32_t> cachedSPIRV;
};