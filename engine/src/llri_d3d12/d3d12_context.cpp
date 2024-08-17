#include "mercury_api.h"
#ifdef MERCURY_GRAPHICS_API_D3D12
#include "d3d12_llri.h"
#include "../context.h"

using namespace llri;

void context::Flush()
{

}

void context::Finish()
{

}

context context::GetCurrent()
{
	context result;
	result.impl = gCurrentCommandBuffer;
	return result;
}

void context::SetPSO(mercury::u32 handle)
{

}

void context::Draw(mercury::u32 count)
{

}

void context::SetConstantsData(void* ptr, mercury::u8 size)
{

}

void context::SetBuffer(mercury::u32 handle)
{

}

void context::DecompressBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::rendering::DecompressPage* pages, int pagesCount)
{

}

void context::CopyBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::u64 size)
{

}
#endif