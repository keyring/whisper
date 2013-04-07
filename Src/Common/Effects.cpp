/******************************************************************************
 *
 * 	Effects.cpp by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *****************************************************************************/

#include "Effects.h"


/*---- Effect ----*/

Effect::Effect(ID3D11Device *device, const std::wstring &filename){

	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory( &compiledShader[0], size, 0, device, &mFX));
}

Effect::~Effect(){

	SafeRelease(mFX);
}


/*---- BasicEffect ----*/

BasicEffect::BasicEffect(ID3D11Device *device, const std::wstring &filename)
	:Effect(device, filename){

		Light1Tech = mFX->GetTechniqueByName("Light1");
		Light2Tech = mFX->GetTechniqueByName("Light2");
		Light3Tech = mFX->GetTechniqueByName("Light3");

		worldViewProjMat     = mFX->GetVariableByName("g_worldViewProj")->AsMatrix();
		worldMat             = mFX->GetVariableByName("g_world")->AsMatrix();
		worldInvTransposeMat = mFX->GetVariableByName("g_worldInvTranspose")->AsMatrix();
		texTransformMat      = mFX->GetVariableByName("g_texTransform")->AsMatrix();

		eyePosVec           = mFX->GetVariableByName("g_eyePos")->AsVector();

		dirLights           = mFX->GetVariableByName("g_dirLights");
		material            = mFX->GetVariableByName("g_material");

		diffuseMap          = mFX->GetVariableByName("g_diffuseMap")->AsShaderResource();
		cubeMap             = mFX->GetVariableByName("g_cubeMap")->AsShaderResource();
}

BasicEffect::~BasicEffect(){

}


/*---- SkyEffect ----*/

SkyEffect::SkyEffect(ID3D11Device *device, const std::wstring &filename)
	:Effect(device, filename){

		SkyTech          = mFX->GetTechniqueByName("SkyDome");
		worldViewProjMat = mFX->GetVariableByName("g_worldViewProj")->AsMatrix();
		cubeMap          = mFX->GetVariableByName("g_cubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect(){

}


/*---- GrassEffect ----*/

GrassEffect::GrassEffect(ID3D11Device *device, const std::wstring &filename)
	:Effect(device, filename){

		GrassTech           = mFX->GetTechniqueByName("GrassRender");

		worldViewProjMat = mFX->GetVariableByName("g_worldViewProj")->AsMatrix();

		timeVar          = mFX->GetVariableByName("g_time")->AsScalar();

		diffuseMap       = mFX->GetVariableByName("g_diffuseMap")->AsShaderResource();
}

GrassEffect::~GrassEffect(){

}


/*---- Effects ----*/

BasicEffect *Effects::BasicFX = 0;
SkyEffect *Effects::SkyFX = 0;
GrassEffect *Effects::GrassFX = 0;

void Effects::InitAll(ID3D11Device *device){

	BasicFX = new BasicEffect(device, L"Src/FX/Basic.fxo");
	SkyFX   = new SkyEffect(device, L"Src/FX/Sky.fxo");
	GrassFX = new GrassEffect(device, L"Src/FX/Grass_instancing.fxo");
}

void Effects::DestroyAll(){

	SafeDelete(BasicFX);
	SafeDelete(SkyFX);
	SafeDelete(GrassFX);
}

