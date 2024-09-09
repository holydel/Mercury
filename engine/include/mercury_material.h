#pragma once
#include "mercury_api.h"
#include "mercury_shader.h"
#include <vector>

namespace mercury
{
	enum class Multisample
	{
		SAMPLES_1
		, SAMPLES_2
		, SAMPLES_4
		, SAMPLES_8
		, SAMPLES_16
	};

	enum class Format
	{
		R32G32B32A32_SFLOAT
		,R32G32B32_SFLOAT
		, R16G16B16A16_SFLOAT
		, R16G16B16_SFLOAT
		,R5G6B5_UNORM_PACK16
		, R32G32_SFLOAT
		, R8G8B8A8_UNORM
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

					const char* semanticName = nullptr;
				};

				std::vector<VertexAttribInfo> inputs;

				void AddAttrib(Format fmt, u8 offset, u8 location, u8 bindingPoint, const char* semanticName = nullptr)
				{
					inputs.emplace_back(VertexAttribInfo{ fmt , offset , location , bindingPoint, semanticName });
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

			bool writeDepth = false;
			bool testDepth = false;
		};

		static Material Create(Desc desc);
	};

	namespace utils
	{
		u32 GetFormatSizeBits(Format fmt);
	}
}