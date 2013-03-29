/******************************************************************************
 *
 * 	Light.fx by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with VS2010 :)
 *
 *	Structures and functions for lighting calcualtions.
 *****************************************************************************/

 /*---- structures ----*/

struct DirectLight{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Direction;
	float unuse;
};

struct PointLight{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Attenuation;
	float unuse;
};

struct SpotLight{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Attenuation;
	float unuse;
};

struct Material{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Reflect;
};


/*---- functions for lighting calculation----*/

void ComputeDirectLight( Material mater,
						DirectLight L,
						float3 normal,
						float3 toEye,
						out float4 ambient,
						out float4 diffuse,
						out float4 specular){
	
	ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -L.Direction;

	ambient = mater.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVec, normal);

	//Flatten to avoid dynamic branching.
	[flatten]
	if(diffuseFactor > 0.0f){
		float v          = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mater.Specular.w);
		
		diffuse  = diffuseFactor * mater.Diffuse * L.Diffuse;
		specular = specFactor * mater.Specular * L.Specular;
	}
}

void ComputePointLight( Material mater,
						PointLight L,
						float3 pos,
						float3 normal,
						float3 toEye,
						out float4 ambient,
						out float4 diffuse,
						out float4 specular){

	ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -L.Position - pos;

	float dist = length(lightVec);

	if(dist > L.Range)
		return;

	lightVec /= dist;	//normalize the vector

	ambient = mater.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if(diffuseFactor > 0.0f){
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mater.Specular.w);

		diffuse  = diffuseFactor * mater.Diffuse * L.Diffuse;
		specular = specular * mater.Specular * L.Specular;
	}

	//Attenuation
	float att = 1.0f / dot(L.Attenuation, float3(1.0f, dist, dist * dist));

	diffuse  *= att;
	specular *= att;
}

void ComputeSpotLight ( Material mater,
						SpotLight L,
						float3 pos,
						float3 normal,
						float3 toEye,
						out float4 ambient,
						out float4 diffuse,
						out float4 specular){

	ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = L.Position - pos;

	float dist = length(lightVec);

	if(dist > L.Range)
		return;

	lightVec /= dist;

	ambient = mater.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if(diffuseFactor > 0.0f){
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mater.Specular.w);

		diffuse  = diffuseFactor * mater.Diffuse * L.Diffuse;
		specular = specFactor * mater.Specular * L.Specular;
	}

	//Scale by spotlight factor and attenuate.
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

	//Scale by spotlight factor and attenuate.
	float att = spot / dot(L.Attenuation, float3(1.0f, dist, dist * dist));

	ambient *= spot;
	diffuse *= att;
	diffuse *= att;
}

