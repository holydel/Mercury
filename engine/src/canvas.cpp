#include "mercury_canvas.h"
#include "mercury_rendering.h"
#include "mesh_internal.h"

#include <glm.hpp>

void mercury::canvas::DrawDedicatedRect(Material mat, float x, float y, float width, float height, glm::vec4 color)
{
	struct RectData
	{
		glm::vec4 geometry;
		glm::vec4 color;
	} rectData;

	glm::vec4 viewport = rendering::GetCurrentViewport();
	float half_width = viewport.z * 0.5f;
	float half_height = viewport.w * 0.5f;

	rectData.geometry = glm::vec4(x / half_width - 1.0f, y / half_height - 1.0f, width / half_width, height / half_width);
	rectData.color = color;

	rendering::SetPSO(mat);
	rendering::SetConstantsData(&rectData, sizeof(rectData));
	rendering::Draw(4);
}

void mercury::canvas::DrawDedicatedStaticMesh(Material mat, Mesh mesh, const glm::mat4& MVP)
{
	rendering::SetPSO(mat);
	rendering::SetConstantsData(&MVP, sizeof(MVP));

	auto const& m = gAllMeshes[mesh.handle];
	rendering::SetBuffer(m.vertexBuffer);
	rendering::SetIndexBuffer(m.indexBuffer);
	rendering::DrawIndexed(m.numIndices);
}