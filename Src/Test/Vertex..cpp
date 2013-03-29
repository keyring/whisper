/******************************************************************************
 *
 * 	Vertex.cpp by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *****************************************************************************/

#include "Vertex.h"
#include "Effects.h"

/*----InputLayoutDesc----*/

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Pos[1] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Basic[3] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};


/*----InputLayouts----*/
ID3D11InputLayout *InputLayouts::Pos = 0;
ID3D11InputLayout *InputLayouts::Basic = 0;

void InputLayouts::InitAll(ID3D11Device *device){

	D3DX11_PASS_DESC passDesc;

	/*
	 * Pos
	 */
	Effects::SkyFX->SkyTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Pos, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Pos));

	/*
	 * Basic
	 */
	Effects::BasicFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Basic, 3, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Basic));
}

void InputLayouts::DestroyAll(){

		SafeRelease(Pos);
		SafeRelease(Basic);
}

