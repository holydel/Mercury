#include "mercury_rendering.h"


#include "llri.h"
#include "swapchain.h"
#include "context.h"

using namespace mercury;

glm::vec4 gCurrentViewport = glm::vec4(0, 0, 1, 1);

void rendering::SetBackGroundColor(float r, float g, float b, float a)
{
	llri::swapchain::setBackGroundColor(r, g, b, a);
}

Shader Shader::CreateFromByteCode(ByteCode bc)
{
	return llri::create_shader_from_bytecode(bc);
}

Material Material::Create(Desc desc)
{
	return llri::create_material(desc);
}

Buffer Buffer::Create(u64 size, HeapType heap)
{
	return llri::create_buffer(size, heap);
}


void rendering::SetPSO(Material mat)
{
	llri::context::GetCurrent().SetPSO(mat.handle);
}

void rendering::Draw(int vert_count)
{
	llri::context::GetCurrent().Draw(vert_count);
}

void rendering::SetConstantsData(void* ptr, u8 size)
{
	llri::context::GetCurrent().SetConstantsData(ptr, size);
}

void rendering::SetBuffer(Buffer buff)
{
	llri::context::GetCurrent().SetBuffer(buff.handle);
}

void* mercury::Buffer::MappedData()
{
	return llri::buffer_mapped_data(handle);
}
		
void rendering::DecompressBuffer(Buffer src, Buffer dst, rendering::DecompressPage* pages, int pagesCount)
{
	llri::context::GetCurrent().DecompressBuffer(src.handle, dst.handle, pages, pagesCount);
}

void rendering::CopyBuffer(Buffer src, Buffer dst, u32 size)
{
	llri::context::GetCurrent().CopyBuffer(src.handle, dst.handle, size);
}

glm::vec4 rendering::GetCurrentViewport()
{
	return gCurrentViewport;
}