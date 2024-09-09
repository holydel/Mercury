#pragma once
#include "mercury_material.h"
#include "glm.hpp"

namespace mercury
{
	class FramePass
	{
	public:
		Format finalFormat = Format::R8G8B8A8_UNORM;
		Multisample numSamples = Multisample::SAMPLES_1;
		bool useDepth = true;
		bool useStencil = false;
	};

	class FrameGraph
	{
	public:		
		bool Build();
		void Set();
	};
}