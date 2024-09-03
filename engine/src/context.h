#pragma once

#include "mercury_api.h"
#include "mercury_rendering.h"

namespace llri
{
	struct context
	{
		void* impl;

		void Flush();
		void Finish();

		static context GetCurrent();
		void SetPSO(mercury::u32 handle);
		void Draw(mercury::u32 count);
		void DrawIndexed(mercury::u16 count);
		void SetConstantsData(const void* ptr, mercury::u8 size);
		void SetBuffer(mercury::u32 handle);
		void SetIndexBuffer(mercury::u32 handle);

		void DecompressBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::rendering::DecompressPage* pages, int pagesCount);
		void CopyBuffer(mercury::u32 srcHandle, mercury::u32 dstHandle, mercury::u64 size);
	};
}