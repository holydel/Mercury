#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "mercury_shader.h"

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
	mercury::Shader::Stage stage = mercury::Shader::Stage::UNKNOWN;

	std::string entryPoint = "main";	
	std::string cachedSource;
	std::vector<uint32_t> cachedSPIRV;

	void CalculateStage();
};