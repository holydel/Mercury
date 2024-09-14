struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

struct FragmentInput
{
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float4 ndc : SV_POSITION;
};

cbuffer ConstantBuffer : register(b0, space0)
{
    float4x4 MVP[2];
};

FragmentInput mainVS(VertexInput vtx, uint viewID : SV_ViewID)
{
	FragmentInput output;
	output.uv = vtx.uv;
	output.normal = vtx.normal;
	output.color = vtx.color;
	output.ndc = mul(MVP[viewID],float4(vtx.position,1));
	return output;
}


float4 mainPS(FragmentInput vtx) : SV_TARGET0
{
	return float4(vtx.uv,vtx.normal.z,1.0f) * vtx.color;
}









