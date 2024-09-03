#pragma once
#include "mercury_material.h"
#include "mercury_mesh.h"
#include "glm.hpp"

namespace mercury
{
	namespace canvas
	{
		void DrawDedicatedRect(Material mat, float x, float y, float width, float height, glm::vec4 color = glm::vec4(1,1,1,1));
		void DrawDedicatedStaticMesh(Material mat, Mesh mesh, const glm::mat4& MVP);
	}
}