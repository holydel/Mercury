struct VSOUT
	{
		float4 pos : SV_POSITION;
		float3 color : COLOR0;
	};

	float4 OutputNDC(float4 ndc)
	{
		return float4(ndc.x, -ndc.y, ndc.zw);
	}
	
	VSOUT ScreenPassVS(uint vID : SV_VertexID)
	{
		VSOUT vertex;
		float2 positions[3] = { { 0.7f, 0.7f }, { -0.7f, 0.7f }, { 0.0f, -0.7f } };
		float3 colors[3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f,1.0f } };

		vertex.color = colors[vID];
		vertex.pos = OutputNDC(float4(positions[vID], 0.0f, 1.0f));

		return vertex;
	}

	float4 GammaPassPS(VSOUT vtx)
		: SV_TARGET0
	{
		return float4(vtx.color, 1.0);
	}




