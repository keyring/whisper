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


static ID3D11Buffer* CreateVertexBuffer( ID3D11Device *device, UINT size, bool dynamic, bool streamout, D3D11_SUBRESOURCE_DATA *pData){

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	if(streamout)
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	else
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if(dynamic){
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}

	ID3D11Buffer *pBuffer = 0;
	HR(device->CreateBuffer( &desc, pData, &pBuffer ));

	return (pBuffer);
}

static ID3D11Buffer* CreateIndexBuffer( ID3D11Device *device, UINT size, bool dynamic, D3D11_SUBRESOURCE_DATA *pData){

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	if(dynamic){
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}

	ID3D11Buffer *pBuffer = 0;
	HR( device->CreateBuffer( &desc, pData, &pBuffer));

	return (pBuffer);
}


Sky::Sky(ID3D11Device *device, const std::wstring &cubeMapFilename, float skySphereRadius){

	HR(D3DX11CreateShaderResourceViewFromFile(device, cubeMapFilename.c_str(), NULL, NULL, &m_skySRV, NULL));

	BasicGeo::MeshData sphere;
	BasicGeo geo;
	geo.CreateSphere(skySphereRadius, 30, 30, sphere);

	const UINT vertexNum = sphere.Vertices.size();
	std::vector<XMFLOAT3> vertices(vertexNum);

	for(size_t i = 0; i < vertexNum; ++i){
		vertices[i] = sphere.Vertices[i].Position;
	}

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &vertices[0];

	m_VB = CreateVertexBuffer( device, sizeof(XMFLOAT3)*vertices.size(), false, false, &vInitData);

	m_indexCount = sphere.Indices.size();

	std::vector<USHORT> indices16;
	indices16.assign(sphere.Indices.begin(), sphere.Indices.end());

	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &indices16[0];

	m_IB = CreateIndexBuffer( device, sizeof(USHORT)*m_indexCount, false, &iInitData);

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

	Effects::SkyFX->setWorldViewProjMat(WVP);
	Effects::SkyFX->setCubeMap(m_skySRV);

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
