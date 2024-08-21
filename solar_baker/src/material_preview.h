#pragma once
#include <mercury_shader.h>
#include <mercury_material.h>

class MaterialPreview
{
	mercury::Shader vertexShader;
	mercury::Shader fragmentShader;
	mercury::Material::Desc previewMaterialDesc;
	mercury::Material previewMaterial;

	MaterialPreview();
	~MaterialPreview();

public:
	static MaterialPreview& Instance();
	void Draw();
	void UpdateShader(std::vector< mercury::u32>& spirv, mercury::Shader::Stage stage);
};