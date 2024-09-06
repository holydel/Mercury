#include "d3d12_utils.h"

#ifdef MERCURY_GRAPHICS_API_D3D12
#include <cassert>

using namespace mercury;

DXGI_FORMAT d3d12_utils::GetFormatFromMercuryFormat(mercury::Format fmt)
{
	switch (fmt)
	{
	case Format::R32G32B32A32_SFLOAT:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	case Format::R32G32B32_SFLOAT:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	case Format::R32G32_SFLOAT:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	case Format::R16G16B16A16_SFLOAT:
		return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	case Format::R16G16B16_SFLOAT:
		return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	case Format::R5G6B5_UNORM_PACK16:
		return DXGI_FORMAT::DXGI_FORMAT_B5G6R5_UNORM; //SWIZZLING HAZARD
	}

	assert(!!!"Missed Format!");
	return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
}

D3D12_PRIMITIVE_TOPOLOGY d3d12_utils::GetTopologyFromMercuryTopology(mercury::Topology topology)
{

	//TriangleList,
	//	PointList,
	//	LineList,
	//	TriangleStrip

	static D3D12_PRIMITIVE_TOPOLOGY topologies[] =
	{ D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	,D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST
	,D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST
	,D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };

	u8 index = static_cast<int>(topology);

	assert(index < mercury::utils::array_size(topologies));

	return topologies[index];
}
#endif