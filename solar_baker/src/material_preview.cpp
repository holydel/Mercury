#include "material_preview.h"
#include "mercury_rendering.h"
using namespace mercury;

MaterialPreview::MaterialPreview()
{

}

MaterialPreview::~MaterialPreview()
{

}


MaterialPreview& MaterialPreview::Instance()
{
	static MaterialPreview instance;
	return instance;
}

void MaterialPreview::Draw()
{
	if (!previewMaterial)
	{
		previewMaterialDesc.vertexShader = mercury::internal_shader::TestTriangleVert();
		previewMaterialDesc.fragmentShader = mercury::internal_shader::TestTriangleFrag();

		previewMaterial = previewMaterialDesc.Instantiate();
	}

	rendering::SetPSO(previewMaterial);
	rendering::Draw(3);
}

void MaterialPreview::UpdateShader(std::vector<mercury::u32>& spirv, mercury::Shader::Stage stage)
{
	mercury::Shader::ByteCode bc;
	bc.data = (u8*)spirv.data();
	bc.size = spirv.size() * 4;
	auto shader = mercury::Shader::CreateFromByteCode(bc);
			
	if (stage == Shader::Stage::Vertex)
	{
		previewMaterialDesc.vertexShader = shader;
	}

	if (stage == Shader::Stage::Fragment)
	{
		previewMaterialDesc.fragmentShader = shader;
	}

	previewMaterial = previewMaterialDesc.Instantiate();
}