#pragma once
#include "mercury_shader.h"

namespace internal_shader_bytecode
{
	mercury::Shader::ByteCode TestTriangleVert();
	mercury::Shader::ByteCode TestTriangleFrag();

	mercury::Shader::ByteCode TestPointCacheVert();
	mercury::Shader::ByteCode TestPointCacheFrag();
}