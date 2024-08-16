#pragma once
#include "mercury_api.h"

namespace mercury
{
	struct Buffer : Handle32
	{
	public:		
		void* MappedData();

		enum class HeapType
		{
			DEFAULT
			,UPLOAD
			,READBACK
		};

		static Buffer Create(u64 size, HeapType heap);
	};
}
