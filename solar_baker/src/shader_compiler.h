#pragma once
#include <vector>
#include <string>
#include "sbproj_source.h"

namespace ShaderCompiler
{
	struct ShaderCompilerErrorInfo
	{
		std::string error;
		mercury::u16 line, col;
	};

	void initialize();
	void shutdown();

	bool CompileShader(SBProjShaderSource * src, std::vector<ShaderCompilerErrorInfo> &errors);
}