/******************************************************************************
 *
 * 	Grass_instancing.fx by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with VS2010 :)
 *
 *	
 *****************************************************************************/
Texture2D g_diffuseMap;

SamplerState samLinear{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbPerFrame{
	float4x4 g_worldViewProj;
    float g_time;
};

BlendState QuadAlphaBlendState{
    AlphaToCoverageEnable = TRUE;
	BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState EnableMSAA{
    CullMode = BACK;
    MultisampleEnable = TRUE;
};

DepthStencilState EnableDepthWrite{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};


struct VertexIn{
    float3 Pos			: POSITION;         //position
    float2 Tex			: TEXCOORD;         //texture coordinate
    float2 vPPos		: vPPos;            // patchPosition
};


struct VertexOut{
    float4 Pos			: SV_POSITION;
    float2 Tex			: TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VertexOut RenderGrassVS( VertexIn vin ){

	VertexOut vout;

	// Out position 
	float4 vGrassPos;
	vGrassPos.xyz = vin.Pos;
	vGrassPos.w   = 1.0; //v.Pos;
	vGrassPos.x  += vin.vPPos.x;
	vGrassPos.z  += vin.vPPos.y;
		
	vout.Pos = mul( vGrassPos, g_worldViewProj );

	// Pass the tex coord through	
	vout.Tex = vin.Tex;
	
	return vout;
}

float4 RenderGrassPS( VertexOut pin) : SV_Target{

    float4 outputColor = g_diffuseMap.Sample( samLinear, pin.Tex );
  
    return outputColor;
}

technique11 GrassRender{

    pass p0{      
	    
        SetVertexShader( CompileShader( vs_5_0,RenderGrassVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, RenderGrassPS() ) );
        
        SetBlendState( QuadAlphaBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( EnableMSAA );
        SetDepthStencilState( EnableDepthWrite, 0 );
    }
}


