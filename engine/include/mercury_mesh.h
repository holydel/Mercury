#pragma once
#include "mercury_api.h"
#include <vector>
#include <glm.hpp>

namespace mercury
{
	namespace vertex_format
	{
		struct StaticVertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
			glm::vec4 color;
		};
	};

	struct Mesh : Handle32
	{
		void* _getVertex(int index, int stride);
	public:
		template<typename T>
		T& Vertex(int index)
		{
			return *(T*)_getVertex(index, sizeof(T));
		}

		static Mesh CreateFromExistsData(void* vertices, void* indices, int vb_size, int ib_size);
	};

	namespace parametrical_meshes
	{
		Mesh CreateCube(float size);
	}
}