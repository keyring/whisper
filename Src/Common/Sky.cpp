/**************************************************************************
 *
 * 	Sky.cpp by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *************************************************************************/

#include "Sky.h"
#include "BasicGeo.h"
#include "Camera.h"
#include "Vertex.h"
#include "Effects.h"

Sky::Sky(ID3D11Device *device, const std::wstring &cubeMapFilename, float skySphereRadius){

	HR(D3DX11CreateShaderResourceViewFromFile(device, cubeMapFilename.c_str(), NULL, NULL, &m_skySRV, NULL));

	BasicGeo::MeshData sphere;
	BasicGeo geo;
	geo.CreateSphere(skySphereRadius, 30, 30, sphere);

	const int vertexNum = sphere.Vertices.size();
	std::vector<XMFLOAT3> vertices(vertexNum);

	for(size_t i = 0; i < vertexNum; ++i){
		vertices[i] = sphere.Vertices[i].Position;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage       = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth   = sizeof(XMFLOAT3) * vertices.size();
	vbd.BindFlags   = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags   = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &vertices[0];

	HR(device->CreateBuffer(&vbd, &vInitData, &m_VB));

	m_indexCount = sphere.Indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage       = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth   = sizeof(USHORT) * m_indexCount;
	ibd.BindFlags   = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags   = 0;

	std::vector<USHORT> indices16;
	indices16.assign(sphere.Indices.begin(), sphere.Indices.end());

	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &indices16[0];

	HR(device->CreateBuffer(&ibd, &iInitData, &m_IB));
}

Sky::~Sky(){

	SafeRelease(m_VB);
	SafeRelease(m_IB);
	SafeRelease(m_skySRV);
}

ID3D11ShaderResourceView *Sky::CreateSkySRV(){
	
	return m_skySRV;
}

void Sky::Draw(ID3D11DeviceContext *dc, const Camera &camera){

	XMFLOAT3 eyePos = camera.getPosition();
	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	
	XMMATRIX WVP = XMMatrixMultiply(T, camera.getViewProjMat());

	Effects::SkyFX->SetWorldViewProj(WVP);
	Effects::SkyFX->SetCubeMap(m_skySRV);

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	dc->IASetIndexBuffer(m_IB, DXGI_FORMAT_R16_UINT, 0);
	dc->IASetInputLayout(InputLayouts::Pos);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::SkyFX->SkyTech->GetDesc(&techDesc);

	for(UINT p = 0; p < techDesc.Passes; ++p){
		ID3DX11EffectPass *pass = Effects::SkyFX->SkyTech->GetPassByIndex(p);
		pass->Apply(0, dc);
		dc->DrawIndexed(m_indexCount, 0, 0);
	}
}
