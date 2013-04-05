/******************************************************************************
 *
 * 	Sky.fx by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with VS2010 :)
 *
 *	SkyDome Effect
 *****************************************************************************/

cbuffer cbPerFrame{
	float4x4 g_worldViewProj;
};

TextureCube g_cubeMap;

SamplerState samTriLinearSam{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

RasterizerState NoCull{
	CullMode = None;
};

DepthStencilState LessEqualDSS{
	DepthFunc = LESS_EQUAL;
};

struct VertexIn{
	float3 PosL : POSITION;
};

struct VertexOut{
	float4 PosH :SV_POSITION;
	float3 PosL :POSITION;
};

VertexOut VS(VertexIn vin){

	VertexOut vout;

	//Set z=w so that z/w=1 (skydome always on far plane)
	vout.PosH = mul(float4(vin.PosL, 1.0f), g_worldViewProj).xyww;

	//Use local vertex position as cubemap lookup vector.
	vout.PosL = vin.PosL;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target{

	return g_cubeMap.Sample(samTriLinearSam, pin.PosL);
}

technique11 SkyDome{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));

		SetRasterizerState(NoCull);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}