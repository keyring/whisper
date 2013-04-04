/*****************************************************************************
 *
 * 	InstancedBillboard.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *****************************************************************************/

#ifndef __INSTANCEDBILLBOARD_H__
#define __INSTANCEDBILLBOARD_H__ 1

#include "AppUtil.h"

template <class T_VERTEX, class T_INSTANCE>
class InstancedBillboard{

	public:
		virtual void Init(std::wstring texName,
			       	T_INSTANCE *inInstanceData,
			       	int count,
			       	T_VERTEX *inVertices,
					int numVerts,
					D3D11_INPUT_ELEMENT_DESC *layout,
					int numLayoutElem,
					WORD *indices = 0,
					int numIndices = 0);

		virtual void Destroy(){};

		virtual HRESULT CreateDevice(ID3D11Device *device, ID3D11DeviceContext* dc, ID3DX11EffectTechnique *tech);
		virtual void DestroyDevice();

		virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectShaderResourceVariable *diffuseVar);
		virtual void SetInstData(ID3D11DeviceContext* dc,T_INSTANCE *inData, int numInstances);

	private:
		int m_vertexCount;
		T_VERTEX *m_vertices;
		ID3D11Buffer *m_vertexBuffer;

		D3D11_INPUT_ELEMENT_DESC* m_layout;
		int m_numElem;
		ID3D11InputLayout* m_vertexLayout;

		std::wstring m_textureName;
		ID3D11ShaderResourceView *m_textureSRV;
		ID3DX11EffectTechnique *m_renderTech;

		int m_instanceCount;
		int m_maxInstCount;
		T_INSTANCE *m_instData;
		ID3D11Buffer *m_instBuffer;

		HRESULT LoadInstData(ID3D11Device *device, ID3D11DeviceContext* dc, ID3D11Buffer **instData, void *inData, int dataSize);

};

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::SetInstData(ID3D11DeviceContext* dc, T_INSTANCE *inData, int numInstances){

	m_instanceCount = numInstances;
	D3D11_MAPPED_SUBRESOURCE ms;
	dc->Map(m_instBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, (void*)inData, sizeof(T_INSTANCE) * m_instanceCount);
	dc->Unmap(m_instBuffer, NULL);
}

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::Init(
	std::wstring texName,
	T_INSTANCE *inInstanceData,
	int count, 
	T_VERTEX *inVertices,
	int numVerts,
	D3D11_INPUT_ELEMENT_DESC *layout,
	int numLayoutElem,
	WORD *indices,
	int numIndices){

		m_textureName = texName;
		m_vertices    = inVertices;
		m_layout      = layout;
		m_numElem     = numLayoutElem;
		m_instData    = inInstanceData;
		m_instanceCount = count;
		m_maxInstCount = count;
}

template <class T_VERTEX, class T_INSTANCE>
HRESULT InstancedBillboard<T_VERTEX, T_INSTANCE>::CreateDevice(ID3D11Device *device, ID3D11DeviceContext* dc, ID3DX11EffectTechnique *tech){

	HRESULT hr = S_OK;
	m_renderTech = tech;

	//Create inputlayout   (later move in Vertex.h/cpp)
	D3DX11_PASS_DESC passDesc;
	m_renderTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(m_layout, m_numElem, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_vertexLayout));

	//Initialize Vertex Buffers
	D3D11_BUFFER_DESC vbd;
	vbd.Usage            = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth        = sizeof(T_VERTEX) * m_vertexCount;
	vbd.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags   = 0;
	vbd.MiscFlags        = 0;
	

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem  = m_vertices;

	HR(device->CreateBuffer(&vbd, &vInitData, &m_vertexBuffer));

	//Create Instance Buffer
	//we create this buffer as dynamic because the instance data in a game could be dynamic
	//we could have moving trees.
	D3D11_BUFFER_DESC bufferDesc = {
		sizeof(T_INSTANCE) * m_instanceCount,
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0,
		0
	};

	HR(device->CreateBuffer(&bufferDesc, NULL, &m_instBuffer));

	//load the texture
	HR(D3DX11CreateShaderResourceViewFromFile(device, m_textureName.c_str(), NULL, NULL, &m_textureSRV, NULL));

	//Initialize instance buffer with data passed in.
	SetInstData(dc, m_instData, m_instanceCount);

	return hr;
}

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::DestroyDevice(){

	SafeRelease(m_vertexBuffer);
	SafeRelease(m_vertexLayout);
	SafeRelease(m_textureSRV);
	SafeRelease(m_instBuffer);

}

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::Draw(ID3D11DeviceContext* dc, ID3DX11EffectShaderResourceVariable *diffuseVar){

	//set the inputlayout
	dc->IASetInputLayout(m_vertexLayout);

	ID3D11Buffer *mvb[2];
	UINT strides[2];
	UINT offsets[2] = {0, 0};
	mvb[0] = m_vertexBuffer;
	mvb[1] = m_instBuffer;
	strides[0] = sizeof(T_VERTEX);
	strides[1] = sizeof(T_INSTANCE);
	dc->IASetVertexBuffers(0,
				   2,
				   mvb,
				   strides,
				   offsets);

	//set primitive topology
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	diffuseVar->SetResource(m_textureSRV);

	//render instanced billboards
	D3DX11_TECHNIQUE_DESC techDesc;
	m_renderTech->GetDesc(&techDesc);

	for(UINT p = 0; p < techDesc.Passes; ++p){
		m_renderTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawInstanced(m_vertexCount,   /*number of vertex per instance */
						m_instanceCount,    /* number of instances */
						0,				/* index of the first vertex */
						0);				/* index of the first instance */
	}
}

template <class T_VERTEX, class T_INSTANCE>
HRESULT InstancedBillboard<T_VERTEX, T_INSTANCE>::LoadInstData(ID3D11Device *device, ID3D11DeviceContext* dc, ID3D11Buffer **inInstData, void *inData, int dataSize){

	HRESULT hr = S_OK;

	// create a resource with the input matrices
	// we create this buffer as dynamic because the instance data in a game could be dynamic
	D3D11_BUFFER_DESC bufferDesc = {
		dataSize,
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0,
		0
	};

	HR(device->CreateBuffer(&bufferDesc, NULL, inInstData));

	D3D11_MAPPED_SUBRESOURCE ms;
	dc->Map((*inInstData), NULL, D3D11_MAP_WRITE_SIDCARD, NULL, &ms);
	memcpy(ms.pData, inData, dataSize);
	dc->Unmap((*inInstData), NULL);

	return hr;
}

#endif    //__INSTANCEDBILLBOARD_H__
