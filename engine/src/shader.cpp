#include "mercury_shader.h"

const char** mercury::Shader::GetHumanReadableShaderNames()
{
	static const char* shaderNames[] =
	{
			  "Vertex"
			, "TessControl"
			, "TessEvaluation"
			, "Geometry"
			, "Fragment"
			, "Compute"
			, "RayGen"
			, "AnyHit"
			, "ClosestHit"
			, "Miss"
			, "Intersect"
			, "Callable"
			, "Task"
			, "Mesh"
			, "SubpassShading"
			, "ClusterCulling"
	};

	return shaderNames;
}