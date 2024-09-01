#include "mercury_mesh.h"
#include "mercury_buffer.h"
using namespace mercury;

struct MeshImpl
{
	Buffer vertexBuffer;
	Buffer indexBuffer;
};

std::vector<MeshImpl> gAllMeshes;

Mesh Mesh::CreateFromExistsData(void* vertices, void* indices, int vb_size, int ib_size)
{
	MeshImpl result;
	u32 resultIndex = gAllMeshes.size();

	result.indexBuffer = Buffer::Create(ib_size, Buffer::HeapType::UPLOAD);
	result.vertexBuffer = Buffer::Create(vb_size, Buffer::HeapType::UPLOAD);

	memcpy(result.indexBuffer.MappedData(), indices, ib_size);
	memcpy(result.vertexBuffer.MappedData(), vertices, vb_size);

	gAllMeshes.push_back(result);

	return { resultIndex };
}