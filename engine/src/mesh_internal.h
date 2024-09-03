#pragma once
#include "mercury_mesh.h"
#include "mercury_buffer.h"

struct MeshImpl
{
	mercury::Buffer vertexBuffer;
	mercury::Buffer indexBuffer;
	mercury::u16 numIndices;
};

extern std::vector<MeshImpl> gAllMeshes;