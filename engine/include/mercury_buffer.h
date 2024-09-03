#pragma once
#include "mercury_api.h"

namespace mercury
{
	struct Buffer : Handle32
	{
	public:
		void* MappedData();

		enum class HeapType : u8
		{
			DEFAULT
			, UPLOAD
			, READBACK
		};

		enum class BufferType : u8
		{
			STAGING
			, VERTEX
			, INDEX
			, UNIFORM
			, STORAGE
		};

		static Buffer Create(u64 size, HeapType heap, BufferType btype);
	};
}
