/*****************************************************************************
 *
 * 	BBGrassPatch.cpp by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *****************************************************************************/

#include "BBGrassPatch.h"

static const D3D11_INPUT_ELEMENT_DESC grassLayout[3] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"vPPos",    0, DXGI_FORMAT_R32G32_FLOAT,    1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1}
};

static const UINT numGrassLayoutElem = sizeof(grassLayout) / sizeof(grassLayout[0]);

BBGrassPatch::BBGrassPatch(int numBB, int inPatchCount, XMFLOAT2* inPatchPos){

	m_numGrassBB = numBB;
	m_patchPos = inPatchPos;
	m_patchCount = inPatchCount;
}

BBGrassPatch::~BBGrassPatch(){

	SafeDeleteArray(m_patchPos);
	SafeDeleteArray(m_visiblePatches);
	SafeDeleteArray(m_grassVerts);
}

HRESULT BBGrassPatch::CreateDevice(ID3D11Device *device, ID3D11DeviceContext* dc, ID3DX11Effect *effect){

	HRESULT hr = S_OK;
	__super::Init(L"Resources/Textures/grassY.dds", 
			m_patchPos,
		    m_patchCount,
			m_grassVerts,
			6 * m_numGrassBB,
			(D3D11_INPUT_ELEMENT_DESC*)grassLayout,
			numGrassLayoutElem);
	ID3DX11EffectTechnique * renderTech = effect->GetTechniqueByName("RenderGarss");
	hr = __super::CreateDevice(device, dc, renderTech);

	return hr;
}

void BBGrassPatch::DestroyDevice(){

	InstancedBillboard::DestroyDevice();
}

void BBGrassPatch::GeneratePatch(int bladeCount, float scale){

	m_numGrassBB = bladeCount;
	m_grassVerts = new BBGrassVertex[m_numGrassBB * 6];

	XMMATRIX mat = XMMatrixIdentity();

	for(int j = 0; j < m_numGrassBB; ++j){
		//Generate each quad at random position, orientation, height
		XMMATRIX matRandom = XMMatrixIdentity();
		float angle = ((float)rand() / RAND_MAX - 0.5f) * 2 * XM_PI; // angle = [-pi, pi]
		float dx = ((float)rand() / RAND_MAX - 0.5f) * 2 * scale;
		float dz = ((float)rand() / RAND_MAX - 0.5f) * 2 * scale;
		float heightScale = ((float)rand() / RAND_MAX - 0.5f) / 2.0f + 1.0f;

		mat = XMMatrixRotationY(angle);
		matRandom = XMMatrixMultiply(matRandom, mat);
		mat = mat * XMMatrixTranslation(dx, 0.0f, dz);
		matRandom = XMMatrixMultiply(matRandom, mat);
		mat = mat * XMMatrixScaling(1.0f, 1.0f, heightScale);
		matRandom = XMMatrixMultiply(matRandom, mat);

		/* Apply the transformation to each vertex of quad */
		for(int i = 0; i < 6; ++i){
			XMVECTOR pos, outPos;
			pos = XMLoadFloat3(&(grassBBVerts[i].Pos));
			pos = XMVectorSetW(pos, 1.0f);
			outPos = XMVector3TransformCoord(pos, matRandom);

			int index = j * 6 + i;
			XMStoreFloat3(&(m_grassVerts[index].Pos), outPos);
			m_grassVerts[index].Texcoord = grassBBVerts[i].Texcoord;
		}
	}
}

void BBGrassPatch::Cull( ID3D11DeviceContext* dc, float fov, XMVECTOR eyePt, XMVECTOR look){

	int numVisibleInstances = 0;
	XMVECTOR patchCenter3D;
	bool visible;

	for(int i = 0; i< m_patchCount; ++i){
		patchCenter3D = XMVectorSetX(patchCenter3D,m_patchPos[i].x);
		patchCenter3D = XMVectorSetZ(patchCenter3D,m_patchPos[i].y);
		patchCenter3D = XMVectorSetY(patchCenter3D,0.0f); /* we only store x,z position for the patches */

		/* we want to cull the patch with a sphere of radius = to 
		 * diagonal of square patch hence radiusPatchBoundingSphere =
		 * 1.414 * rpatch
		 */
	       visible = SphereConeTest(patchCenter3D, m_patchRadius * 1.414f, fov, eyePt, look);
	       if(visible){
		       //visible add to draw list
		       m_visiblePatches[numVisibleInstances] = m_patchPos[i];
		       numVisibleInstances++;
	       }
	}
	SetInstData(dc, m_visiblePatches, numVisibleInstances);
}

void BBGrassPatch::Generate(XMVECTOR surfaceCenter, float surfaceR, int patchBladeCount, float inPatchRadius){

	m_patchRadius = inPatchRadius;
	GeneratePatch(patchBladeCount, m_patchRadius);

	m_patchCountX = int(surfaceR / m_patchRadius);
	m_patchCountZ = int(surfaceR / m_patchRadius);
	m_patchCount = (2 * m_patchCountX - 1) * (2 * m_patchCountZ - 1);
	m_patchPos = new XMFLOAT2[m_patchCount];
	m_visiblePatches = new XMFLOAT2[m_patchCount];

	int k = 0;
	for(int i = -(m_patchCountX - 1); i < m_patchCountX; ++i){
		for(int j = -(m_patchCountZ - 1); j < m_patchCountZ; ++j){
			m_patchPos[k].x = XMVectorGetX(surfaceCenter) + i * m_patchRadius;
			m_patchPos[k].y = XMVectorGetZ(surfaceCenter) + j * m_patchRadius;
			k++;
		}
	}
}
