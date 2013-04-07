//--------------------------------------------------------------------------------------
// File: Grass.fx
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D g_diffuseMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbNeverChanges
{
	float4	g_vWaveDistortX = float4( 3.0f, 0.4f, 0.0f, 0.3f );
	float4	g_vWaveDistortZ = float4( 3.0f, 0.4f, 0.0f, 0.3f );
	float4	g_vWaveDistortY = float4( -1.0f, -0.133f, -0.333f, -0.10f );
	float4	g_vWaveDirX = float4( -0.006f, -0.012f, 0.024f, 0.048f );
	float4	g_vWaveDirZ = float4( -0.003f, -0.006f, -0.012f, -0.048f );
	float4	g_vWaveSpeed = float4( 0.3f, 0.7f, 0.6f, 1.4f );
	float	g_fPIx2 = 6.28318530f;
	float4	g_vLightingWaveScale = float4( 0.35f, 0.10f, 0.10f, 0.03f );
	float4	g_vLightingScaleBias = float4( 0.6f, 0.7f, 0.2f, 0.0f );
};

cbuffer cbPerFrame{
	float4x4 g_worldViewProj;
    float g_time;
};

BlendState QuadAlphaBlendState
{
    AlphaToCoverageEnable = TRUE;
	BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState EnableMSAA
{
    CullMode = BACK;
    MultisampleEnable = TRUE;
};

DepthStencilState EnableDepthWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};


struct VertexIn
{
    float3 Pos			: POSITION;         //position
    float2 Tex			: TEXCOORD;        //texture coordinate
    float2 vPPos		: vPPos;            // patchPosition
};


struct VertexOut
{
    float4 Pos			: SV_POSITION;
    float4 Color		: COLOR0;
    float2 Tex			: TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VertexOut RenderGrassVS( VertexIn vin )
{
	VertexOut vout;
	//sinusoidal vertex motion for waving grass
	//pos + sumOverI(wavedirI * texcoordy * sin( xdirI * (xpos+time)) + ydirI * (ypos+time)))


	// use vertex pos x and y as inputs to sinusoidal warp 
	float4 vWaveVec = (g_vWaveDirX * vin.Pos.x) + (g_vWaveDirZ * vin.Pos.z);

	// add scaled time to move bumps according to speed
	vWaveVec += g_time * g_vWaveSpeed;

	// take frac of all 4 components
	vWaveVec = frac( vWaveVec );

	vWaveVec -= 0.8f;

	// *=2pi coords range from (-pi to pi)
	vWaveVec *= g_fPIx2;	// pi * 2.0
	
	// taylor series expansion replaced by actual sin fun
	vWaveVec = sin( vWaveVec );

	float4 vWaveDistortion;
	vWaveDistortion.x  = dot( vWaveVec, g_vWaveDistortX );
	vWaveDistortion.y  = dot( vWaveVec, g_vWaveDistortY );
	vWaveDistortion.zw = dot( vWaveVec, g_vWaveDistortZ );

	// attenuate sinusoidal warping by (1-tex0.y)^2
	float fSinWarp	 = 1.0f - vin.Tex.y;
	fSinWarp		*= fSinWarp;
	vWaveDistortion *= fSinWarp;

	// Out position -- add sinusoidal warping to grass position
	float4 vGrassPos;
	vGrassPos.xyz = vWaveDistortion + vin.Pos;
	vGrassPos.w   = 1.0; //v.Pos;
	vGrassPos.x  += vin.vPPos.x;
	vGrassPos.z  += vin.vPPos.y;
		
	vout.Pos = mul( vGrassPos, g_worldViewProj );

	// scale and add sin waves together
	// scale and bias color values 
	// (green is scaled more than red and blue)

	float fScaled = dot( vWaveVec, g_vLightingWaveScale );
	vout.Color = (g_vLightingScaleBias.zzzw * -fScaled) + g_vLightingScaleBias.y;
	// Pass the tex coord through	
	vout.Tex = vin.Tex;
	
	return vout;
}

float4 RenderGrassPS( VertexOut pin) : SV_Target
{
    //calculate lighting assuming light color is <1,1,1,1>
    //float fLighting  = saturate( dot( input.Norm, g_LightDir ) );
    float4 outputColor = g_diffuseMap.Sample( samLinear, pin.Tex );
    outputColor.xyz   *= pin.Color.xyz;
    return outputColor;
}

technique11 GrassRender
{
    pass p0
    {          
        SetVertexShader( CompileShader( vs_5_0,RenderGrassVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, RenderGrassPS() ) );
        
        SetBlendState( QuadAlphaBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( EnableMSAA );
        SetDepthStencilState( EnableDepthWrite, 0 );
    }
}


