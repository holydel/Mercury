#include "mercury_shader.h"
#include "shader_bytecode.h"

using namespace mercury;

Shader internal_shader::TestTriangleVert()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::TestTriangleVert());
	return result;
}

Shader internal_shader::TestTriangleFrag()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::TestTriangleFrag());
	return result;
}

Shader internal_shader::TestPointCacheVert()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::TestPointCacheVert());
	return result;
}

Shader internal_shader::TestPointCacheFrag()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::TestPointCacheFrag());
	return result;
}