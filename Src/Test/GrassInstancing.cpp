/**************************************************************************
 *
 * 	Grass.cpp by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *************************************************************************/

#include "GrassInstancing.h"
#include "Camera.h"

static Vertex::Grass grassBBVerts[] = {
	//	            x      y       z			 tu1   tv1    
	{  XMFLOAT3( -10.0f,   0.0f,   0.0f),  XMFLOAT2( 1.0f,  0.01f )},
	{  XMFLOAT3(  10.0f,   0.0f,   0.0f),  XMFLOAT2( 0.01f, 0.01f )},
	{  XMFLOAT3(  10.0f,  -10.0f,  0.0f),  XMFLOAT2( 0.01f, 1.0f ) },

	{  XMFLOAT3(  -10.0f,   0.0f,  0.0f),  XMFLOAT2( 1.0f,  0.01f )},
	{  XMFLOAT3(   10.0f,  -10.0f, 0.0f),  XMFLOAT2( 0.01f, 1.0f ) },
	{  XMFLOAT3(  -10.0f,  -10.0f, 0.0f),  XMFLOAT2( 1.0f,  1.0f ) }, 
};

/* x, z instance positions....*/
static XMFLOAT2 defaultPatchPos[] = {
	//	   x      z					
	XMFLOAT2(  20.0f, 0.0f ) ,
	XMFLOAT2(  0.0f,  0.0f ) ,
	XMFLOAT2( -20.0f, 0.0f ) ,

	XMFLOAT2(  20.0f, -20.0f ) ,
	XMFLOAT2(  0.0f,  -20.0f  ) ,
	XMFLOAT2( -20.0f, -20.0f ) ,

	XMFLOAT2(  20.0f,  40.0f ) ,
	XMFLOAT2(  20.0f,  40.0f ) ,
	XMFLOAT2(  0.0f,   40.0f ) ,
	XMFLOAT2( -20.0f,  40.0f ),
	XMFLOAT2( -40.0,  40.0f ),

	XMFLOAT2(  40.0f,  20.0f ) ,
	XMFLOAT2(  20.0f,  20.0f ) ,
	XMFLOAT2(  0.0f,   20.0f ) ,
	XMFLOAT2( -20.0f,  20.0f ) ,
	XMFLOAT2( -40.0f,  20.0f ) ,
};

const int defaultPatchCount = 16;

Grass::Grass(ID3D11Device *device, 
	ID3D11DeviceContext *dc, 
	const std::wstring &diffuseMapFilename,
	FXMVECTOR surfaceCenter, 
	float surfaceR, 
	int patchBladeCount, 
	float inPatchRadius){

		Generate(surfaceCenter, surfaceR, patchBladeCount, inPatchRadius);

		m_numGrassBB = DEFAULT_NUM_GRASS_BB;
		m_patchPos   = defaultPatchPos;
		m_patchCount = defaultPatchCount;

		m_instData = m_patchPos;
		m_vertexData = m_grassVerts;

		m_instCount = m_patchCount;
		m_maxInstCount = m_patchCount;
		m_vertexCount = 6 * m_numGrassBB;

		HR(D3DX11CreateShaderResourceViewFromFile(device, diffuseMapFilename.c_str(), NULL, NULL, &m_grassSRV, NULL));

		//Initialize Vertex Buffers
		D3D11_BUFFER_DESC vbd;
		vbd.Usage            = D3D11_USAGE_DEFAULT;
		vbd.ByteWidth        = sizeof(Vertex::Grass) * m_vertexCount;
		vbd.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags   = 0;
		vbd.MiscFlags        = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vInitData;
		vInitData.pSysMem  = m_vertexData;

		HR(device->CreateBuffer(&vbd, &vInitData, &m_vertexBuffer));

		/* Create Instance Buffer
		 *we create this buffer as dynamic because the instance data in
		 *a game could be dynamic we could have moving trees.
		*/
		D3D11_BUFFER_DESC bufferDesc = {
		sizeof(XMFLOAT2) * m_instCount,
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0,
		0
	};

	HR(device->CreateBuffer(&bufferDesc, NULL, &m_instBuffer));

	//Initialize instance buffer with data passed in.
	SetInstData(dc, m_instData, m_instCount);
}

Grass::~Grass(){

	SafeRelease(m_vertexBuffer);
	SafeRelease(m_instBuffer);
	SafeRelease(m_grassSRV);
}

ID3D11ShaderResourceView *Grass::CreateGrassSRV(){

	return m_grassSRV;
}

void Grass::Cull( ID3D11DeviceContext* dc, float fov, const Camera &camera){

	int numVisibleInstances = 0;
	XMVECTOR patchCenter3D = XMVectorZero();
	bool visible;

	for(int i = 0; i< m_patchCount; ++i){
		patchCenter3D = XMVectorSetX(patchCenter3D,m_patchPos[i].x);
		patchCenter3D = XMVectorSetZ(patchCenter3D,m_patchPos[i].y);
		patchCenter3D = XMVectorSetY(patchCenter3D,0.0f); /* we only store x,z position for the patches */

		/* we want to cull the patch with a sphere of radius = to 
		 * diagonal of square patch hence radiusPatchBoundingSphere =
		 * 1.414 * rpatch
		 */
		visible = SphereConeTest(patchCenter3D, m_patchRadius * 1.414f, fov, camera.getPositionVec(), camera.getLookVec());
	       if(visible){
		       //visible add to draw list
		       m_visiblePatches[numVisibleInstances] = m_patchPos[i];
		       numVisibleInstances++;
	       }
	}
	SetInstData(dc, m_visiblePatches, numVisibleInstances);
}

void Grass::Draw(ID3D11DeviceContext *dc, const Camera &camera){

	//XMFLOAT3 eyePos = camera.getPosition();
	//XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	XMMATRIX T = XMMatrixIdentity();
	
	XMMATRIX WVP = XMMatrixMultiply(T, camera.getViewProjMat());

	Effects::GrassFX->setWorldViewProjMat(WVP);
	Effects::GrassFX->setDiffuseMap(m_grassSRV);

	ID3D11Buffer *mvb[2];
	UINT strides[2];
	UINT offsets[2] = {0, 0};
	mvb[0] = m_vertexBuffer;
	mvb[1] = m_instBuffer;
	strides[0] = sizeof(Vertex::Grass);
	strides[1] = sizeof(XMFLOAT2);
	dc->IASetVertexBuffers(0, 2, mvb, strides, offsets);
	dc->IASetInputLayout(InputLayouts::Grass);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//render instanced billboards
	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::GrassFX->GrassTech->GetDesc(&techDesc);

	for(UINT p = 0; p < techDesc.Passes; ++p){
		ID3DX11EffectPass *pass = Effects::GrassFX->GrassTech->GetPassByIndex(p);
		pass->Apply(0, dc);
		dc->DrawInstanced(m_vertexCount,   /*number of vertex per instance */
						m_instCount,    /* number of instances */
						0,				/* index of the first vertex */
						0);				/* index of the first instance */
	}
}

void Grass::Generate(FXMVECTOR surfaceCenter, float surfaceR, int patchBladeCount, float inPatchRadius){

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

void Grass::SetInstData(ID3D11DeviceContext* dc, XMFLOAT2 *inData, UINT numInstances){

	m_instCount = numInstances;
	D3D11_MAPPED_SUBRESOURCE ms;
	dc->Map(m_instBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, (void*)inData, sizeof(XMFLOAT2) * m_instCount);
	dc->Unmap(m_instBuffer, NULL);
}

void Grass::GeneratePatch(int bladeCount, float scale){

	m_numGrassBB = bladeCount;
	m_grassVerts = new Vertex::Grass[m_numGrassBB * 6];

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