struct DrawConstants
{
	float4 geometry;
	float4 color;
};

struct VSOUT
{
	float4 pos : SV_POSITION;
};

ConstantBuffer<DrawConstants> drawConstants : register(b0, space0);

VSOUT mainVS(uint vID : SV_VertexID)
{
	float sx = drawConstants.geometry.x - drawConstants.geometry.z;
	float ex = drawConstants.geometry.x + drawConstants.geometry.z;
	float sy = drawConstants.geometry.y - drawConstants.geometry.w;
	float ey = drawConstants.geometry.y + drawConstants.geometry.w;
	
	float2 positions[] = 
	{
		{sx,sy},
		{ex,sy},
		{sx,ey},
		{ex,ey}
	};
	
	VSOUT result;
	result.pos = float4(positions[vID],0,1);
	return result;
}


float4 mainPS(VSOUT vtx) : SV_TARGET0
{
	return drawConstants.color;
}





