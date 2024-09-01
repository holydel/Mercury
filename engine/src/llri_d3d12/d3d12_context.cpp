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

//void context::SetPSO(mercury::u32 handle)
//{
//	
//	vkCmdBindPipeline(cbuff, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, gAllPSOs[handle]);
//}
//
//void context::Draw(mercury::u32 count)
//{
//	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(impl);
//	vkCmdDraw(cbuff, count, 1, 0, 0);
//}

void context::SetPSO(mercury::u32 handle)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	cbuff->SetPipelineState(gAllPSOs[handle]);
	cbuff->SetGraphicsRootSignature(gAllSignatures[handle]);	
}

void context::Draw(mercury::u32 count)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	cbuff->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cbuff->DrawInstanced(count, 1, 0, 0);
	//cbuff->
}

void context::SetConstantsData(void* ptr, mercury::u8 size)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	cbuff->SetGraphicsRoot32BitConstants(0, size / 4, ptr, 0);
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