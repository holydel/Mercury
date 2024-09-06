#pragma once
#include "mercury_api.h"
#include "mercury_utils.h"

#ifdef MERCURY_GRAPHICS_API_D3D12
#include "mercury_material.h"
#include <directx/d3d12.h>

namespace d3d12_utils
{
	DXGI_FORMAT GetFormatFromMercuryFormat(mercury::Format fmt);
	D3D12_PRIMITIVE_TOPOLOGY GetTopologyFromMercuryTopology(mercury::Topology topology);
}

#endif