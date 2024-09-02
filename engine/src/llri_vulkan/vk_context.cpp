#include "mercury_api.h"
#ifdef MERCURY_GRAPHICS_API_VULKAN
#include "vk_llri.h"
#include "../context.h"
#include <cassert>

using namespace llri;
using namespace mercury;

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
	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(impl);
	vkCmdBindPipeline(cbuff, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,gAllPSOs[handle]);
}

void context::Draw(mercury::u32 count)
{
	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(impl);
	vkCmdDraw(cbuff, count, 1, 0, 0);
}

void context::SetConstantsData(void* ptr, u8 size)
{
	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(impl);
	vkCmdPushConstants(cbuff, gAllLayouts[0], VK_SHADER_STAGE_ALL, 0, size, ptr);
}

void context::SetBuffer(mercury::u32 handle)
{
	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(impl);
	VkDeviceSize offset = 0;

	//auto meta = gAllBuffersMeta[handle];
	//vmaFlushAllocation(gVMAallocator, meta.allocation, meta.allocInfo.offset, meta.allocInfo.size);

	vkCmdBindVertexBuffers(cbuff, 0, 1, &gAllBuffers[handle], &offset);
}

void context::DecompressBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::rendering::DecompressPage* pages, int pagesCount)
{
	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(impl);

	auto srcBuff = gAllBuffers[srcHandle];
	auto dstBuff = gAllBuffers[dstHandle];

	VkDecompressMemoryRegionNV decompPages[512];
	for (int i = 0; i < pagesCount; ++i)
	{
		decompPages[i].compressedSize = pages[i].srcSize;
		decompPages[i].decompressedSize = pages[i].dstSize;
		decompPages[i].decompressionMethod = VK_MEMORY_DECOMPRESSION_METHOD_GDEFLATE_1_0_BIT_NV;
		decompPages[i].srcAddress = gAllBuffersMeta[srcHandle].deviceAddress + pages[i].srcOffset;
		decompPages[i].dstAddress = gAllBuffersMeta[dstHandle].deviceAddress + pages[i].dstOffset;

		assert(decompPages[i].srcAddress % 4 == 0);
		assert(decompPages[i].dstAddress % 4 == 0);

		
	}
	vkCmdDecompressMemoryNV(cbuff, pagesCount, decompPages);

	//vkCmdPipelineBarrier(
	//	cbuff,
	//	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // source stage
	//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,    // destination stage
	//	0,0,0,0,0,0,0);

	vkCmdDecompressMemoryNV(cbuff, pagesCount, decompPages);
	
	//vkCmdPipelineBarrier(
	//	cbuff,
	//	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // source stage
	//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,    // destination stage
	//	0, 0, 0, 0, 0, 0, 0);
	//vkCmdPipelineBarrier(cbuff,)
}

void context::CopyBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::u64 size)
{
	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(impl);

	auto srcBuff = gAllBuffers[srcHandle];
	auto dstBuff = gAllBuffers[dstHandle];

	VkBufferCopy region = {};
	region.size = size;

	vkCmdCopyBuffer(cbuff, srcBuff, dstBuff, 1, &region);
}

#endif