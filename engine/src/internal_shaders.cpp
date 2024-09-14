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

Shader internal_shader::ColoredSpriteVS()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::ColoredSpriteVS());
	return result;
}

Shader internal_shader::ColoredSpritePS()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::ColoredSpritePS());
	return result;
}

Shader internal_shader::StaticDedicatedMeshVS()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::StaticDedicatedMeshVS());
	return result;
}

Shader internal_shader::StaticDedicatedMeshPS()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::StaticDedicatedMeshPS());
	return result;
}

Shader internal_shader::StaticDedicatedMeshVS_Stereo()
{
	static Shader result = Shader::CreateFromByteCode(internal_shader_bytecode::StaticDedicatedMeshVS_Stereo());
	return result;
}