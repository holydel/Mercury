#pragma once
#include "mercury_api.h"
#include "mercury_shader.h"
#include <vector>

namespace mercury
{
	enum class Format
	{
		R32G32B32A32_SFLOAT
		,R32G32B32_SFLOAT
		, R16G16B16A16_SFLOAT
		, R16G16B16_SFLOAT
		,R5G6B5_UNORM_PACK16
	};

	enum class BlendMode
	{
		None
		,AlphaBlend
		,PremultiplyAplhaBlend
		,Add
		,Multiply
	};

	struct Material : Handle32
	{
		struct Desc
		{
			struct RenderState
			{
				BlendMode blendMode = BlendMode::None;
			} renderState;

			struct VertexInput
			{
				struct VertexAttribInfo
				{
					Format fmt;
					u8 offset;
					u8 location;
					u8 bindingPoint;
				};

				std::vector<VertexAttribInfo> inputs;

				void AddAttrib(Format fmt, u8 offset, u8 location, u8 bindingPoint)
				{
					inputs.emplace_back(VertexAttribInfo{ fmt , offset , location , bindingPoint });
				}
			} vertexInput;
			

			Shader vertexShader;
			Shader fragmentShader;

			Material Instantiate()
			{
				return Material::Create(*this);
			}
		};

		static Material Create(Desc desc);
	};

	namespace utils
	{
		u32 GetFormatSizeBits(Format fmt);
	}
}