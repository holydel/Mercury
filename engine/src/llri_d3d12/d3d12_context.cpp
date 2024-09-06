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

PSOMeta currentPSOMeta;

void context::SetPSO(mercury::u32 handle)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	cbuff->SetPipelineState(gAllPSOs[handle]);
	cbuff->SetGraphicsRootSignature(gAllSignatures[handle]);
	currentPSOMeta = gAllPSOMetas[handle];
}

void context::Draw(mercury::u32 count)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	cbuff->IASetPrimitiveTopology(currentPSOMeta.topology);
	cbuff->DrawInstanced(count, 1, 0, 0);
	//cbuff->
}

void context::DrawIndexed(mercury::u16 count)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	cbuff->IASetPrimitiveTopology(currentPSOMeta.topology);

	cbuff->DrawIndexedInstanced(count, 1, 0, 0, 0);
}

void context::SetConstantsData(const void* ptr, mercury::u8 size)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	cbuff->SetGraphicsRoot32BitConstants(0, size / 4, ptr, 0);
}

void context::SetBuffer(mercury::u32 handle)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	auto& meta = gAllBuffersMeta[handle];

	D3D12_VERTEX_BUFFER_VIEW view = { meta.bufferGPULocation,meta.sizeInBytes,currentPSOMeta.stride};
	cbuff->IASetVertexBuffers(0, 1, &view);
}

void context::SetIndexBuffer(mercury::u32 handle)
{
	ID3D12GraphicsCommandList* cbuff = static_cast<ID3D12GraphicsCommandList*>(impl);
	auto& meta = gAllBuffersMeta[handle];

	D3D12_INDEX_BUFFER_VIEW view = { meta.bufferGPULocation,meta.sizeInBytes,DXGI_FORMAT::DXGI_FORMAT_R16_UINT};
	cbuff->IASetIndexBuffer(&view);
}

void context::DecompressBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::rendering::DecompressPage* pages, int pagesCount)
{

}

void context::CopyBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::u64 size)
{

}
#endif