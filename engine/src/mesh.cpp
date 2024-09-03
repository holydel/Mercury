#include "mesh_internal.h"

using namespace mercury;

std::vector<MeshImpl> gAllMeshes;

Mesh Mesh::CreateFromExistsData(void* vertices, void* indices, int vb_size, int ib_size)
{
	MeshImpl result;
	u32 resultIndex = gAllMeshes.size();

	result.indexBuffer = Buffer::Create(ib_size, Buffer::HeapType::UPLOAD,Buffer::BufferType::INDEX);
	result.vertexBuffer = Buffer::Create(vb_size, Buffer::HeapType::UPLOAD, Buffer::BufferType::VERTEX);

	memcpy(result.indexBuffer.MappedData(), indices, ib_size);
	memcpy(result.vertexBuffer.MappedData(), vertices, vb_size);
	result.numIndices = ib_size / sizeof(u16);

	gAllMeshes.push_back(result);

	return { resultIndex };
}