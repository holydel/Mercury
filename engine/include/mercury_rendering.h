#pragma once
#include "mercury_api.h"
#include "mercury_material.h"
#include "mercury_buffer.h"

namespace mercury
{
	namespace rendering
	{
		struct DecompressPage
		{
			int srcOffset;
			int srcSize;
			int dstOffset;
			int dstSize;
		};

		void SetBackGroundColor(float r, float g, float b, float a);

		void SetPSO(Material mat);
		void Draw(int tris_count);

		void SetConstantsData(void* ptr, u8 size);
		void SetBuffer(Buffer buff);

		void DecompressBuffer(Buffer src, Buffer dst, DecompressPage* pages, int pagesCount);
		void CopyBuffer(Buffer src, Buffer dst, u32 size);
	}
}