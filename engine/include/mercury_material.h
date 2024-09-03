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
		, R32G32_SFLOAT
	};

	enum class BlendMode
	{
		None
		,AlphaBlend
		,PremultiplyAplhaBlend
		,Add
		,Multiply
	};

	enum class Topology : u8
	{
		TriangleList,
		PointList,
		LineList,
		TriangleStrip
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
			
			struct ShaderInputs
			{
				int numPushConstants = 0; //number of 32bit push constants
			} shaderInputs;

			Shader vertexShader;
			Shader fragmentShader;

			Material Instantiate()
			{
				return Material::Create(*this);
			}

			Topology topology = Topology::TriangleList;
		};

		static Material Create(Desc desc);
	};

	namespace utils
	{
		u32 GetFormatSizeBits(Format fmt);
	}
}