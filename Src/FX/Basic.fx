/******************************************************************************
 *
 * 	Basic.fx by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with VS2010 :)
 *
 *	Basic Effects : lighting, texturing
 *****************************************************************************/

 #include "Light.fx"

 cbuffer cbPerObject{
	float4x4 g_world;
	float4x4 g_worldViewProj;
	float4x4 g_worldInvTranspose;
	float4x4 g_texTransform;
	Material g_material;
};
 
 cbuffer cbPerFrame{
	DirectLight g_directLights[3];
	float3 g_eyePos;
};

Texture2D g_diffuseMap;
TextureCube g_cubeMap;

SamplerState samAnisotropic{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn{
	float3 PosL    : POSITION;
	float3 Normal  : NORMAL;
	float2 Texcoord: TEXCOORD;
};

struct VertexOut{
	float4 PosH      : SV_POSITION;
	float3 PosW      : POSITION;
	float3 NormalW   : NORMAL;
	float2 Texcoord  : TEXCOORD;
}; 

VertexOut VS(VertexIn vin){

	VertexOut vout;

	//Transform to world sapce.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), g_world).xyz;
	vout.NormalW = mul(vin.Normal, (float3x3)g_worldInvTranspose);

	//Transform to homogeneous clip space
	vout.PosH = mul(float4(vin.PosL, 1.0f), g_worldViewProj);

	//output vertex attributes for interpolation across triangle.
	vout.Texcoord = mul(float4(vin.Texcoord, 0.0f, 1.0f), g_texTransform).xy;

	return vout;
}

float4 PS( VertexOut pin,
		   uniform int g_lightCount,
		   uniform bool g_useTexture,
		   uniform bool g_reflectionEnabled) : SV_Target{

	//Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	//The toEye vector is used in lighting.
	float3 toEye = g_eyePos - pin.PosW;

	//cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	//normalize
	toEye /= distToEye;

	//Default to multiplicative identity
	float4 texColor = float4(1, 1, 1, 1);
	if(g_useTexture){
		
		//Sample texture
		texColor = g_diffuseMap.Sample(samAnisotropic, pin.Texcoord);
	}


	/*
	*Lighting
	*/

	float4 finalColor = texColor;

	if(g_lightCount > 0){
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		//sum the light contribution from each light source
		[unroll]
		for(int i = 0; i < g_lightCount; ++i){
			float4 A ,D, S;

			//there is Directional Light
			ComputeDirectLight(g_material, g_directLights[i], pin.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		finalColor = texColor * (ambient + diffuse) + specular;

		if(g_reflectionEnabled){
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, pin.NormalW);
			float4 reflectionColor = g_cubeMap.Sample(samAnisotropic, reflectionVector);

			finalColor += g_material.Reflect * reflectionColor;
		}
	}

	/*
	* Other Effect like Fogging...
	*/

	return finalColor;
}

technique11 Light1{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, false, false)));
	}
}

technique11 Light2{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(2, false, false)));
	}
}

technique11 Light3{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, false, false)));
	}
}

technique11 Light0Tex{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(0, true, false)));
	}
}

technique11 Light1Tex{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, true, false)));
	}
}

technique11 Light2Tex{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(2, true, false)));
	}
}

technique11 Light3Tex{

	pass p0{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, true, false)));
	}
}