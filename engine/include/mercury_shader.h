#pragma once
#include "mercury_api.h"

namespace mercury
{
	struct Shader : Handle32
	{
		enum class Stage : i8
		{
			UNKNOWN = -1,
			Vertex = 0
			, TessControl
			, TessEvaluation
			, Geometry
			, Fragment
			, Compute
			, RayGen
			, AnyHit
			, ClosestHit
			, Miss
			, Intersect
			, Callable
			, Task
			, Mesh
			, SubpassShading
			, ClusterCulling
			, ShaderGraph
			,MAX
		};

		struct ByteCode
		{
			u8* data;
			u32 size;
		};

		static Shader CreateFromByteCode(ByteCode bc); //llri implementation (vulkan/d3d12/metal/WGPU)

		static const char** GetHumanReadableShaderNames(); //shared engine implementation
	};

	namespace internal_shader //shader backend implementation (spirv/dxil/metal)
	{
		Shader TestTriangleVert();
		Shader TestTriangleFrag();
		Shader TestPointCacheVert();
		Shader TestPointCacheFrag();
		Shader ColoredSpriteVS();
		Shader ColoredSpritePS();
		Shader StaticDedicatedMeshVS();
		Shader StaticDedicatedMeshPS();

		Shader StaticDedicatedMeshVS_Stereo();
	}
}