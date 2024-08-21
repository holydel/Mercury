#include "sbproj_source.h"
using namespace mercury;

void SBProjShaderSource::CalculateStage()
{
	struct StageInfo
	{
		Shader::Stage stage;
		std::string ext;
	};

	StageInfo stages[] =
	{
		{Shader::Stage::Vertex,"vert"}
		,{Shader::Stage::TessControl,"tesc"}
		,{Shader::Stage::TessEvaluation,"tese"}
		,{Shader::Stage::Geometry,"geom"}
		,{Shader::Stage::Fragment,"frag"}
		,{Shader::Stage::Compute,"comp"}
		,{Shader::Stage::RayGen,"rgen"}
		,{Shader::Stage::AnyHit,"ahit"}
		,{Shader::Stage::ClosestHit,"chit"}
		,{Shader::Stage::Miss,"miss"}
		,{Shader::Stage::Intersect,"isect"}
		,{Shader::Stage::Callable,"call"}
		,{Shader::Stage::Task,"task"}
		,{Shader::Stage::Mesh,"mesh"}
		,{Shader::Stage::SubpassShading,"subpass"} //not supported for huawei SubpassShading
		,{Shader::Stage::ClusterCulling,"ccull"} //not supported for huawei ClusterCulling
	};

	stage= Shader::Stage::Vertex;
	auto srcext = fullPath.extension().u8string().substr(1);

	for (auto& s : stages)
	{
		if (s.ext == srcext)
		{
			stage = s.stage;
			break;
		}
	}
}
