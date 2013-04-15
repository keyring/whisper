/****************************************************************************
 *
 * 	Grass.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 				--code with gVim :)
 ***************************************************************************/

#ifndef __GRASS_H__
#define __GRASS_H__

#include "AppUtil.h"
#include "Vertex.h"
#include "Effects.h"

#define DEFAULT_NUM_GRASS_BB 100

class Camera;

class Grass{

	public:
		Grass::Grass(ID3D11Device *device, 
			ID3D11DeviceContext *dc, 
			const std::wstring &diffuseMapFilename,
			FXMVECTOR surfaceCenter, 
			float surfaceR, 
			int patchBladeCount, 
			float inPatchRadius);

		~Grass();

		ID3D11ShaderResourceView *CreateGrassSRV();

		void Cull( ID3D11DeviceContext* dc, float fov, const Camera &camera);

		void Draw(ID3D11DeviceContext *dc, const Camera &camera);

		void Generate(FXMVECTOR surfaceCenter, float surfaceR, int patchBladeCount, float inPatchRadius);

		void SetInstData(ID3D11DeviceContext* dc,XMFLOAT2 *inData, UINT numInstances);

	private:
		Grass(const Grass &rhs);
		Grass& operator=(const Grass &rhs);

	private:
		ID3D11Buffer *m_vertexBuffer;
		ID3D11Buffer *m_instBuffer;
		ID3D11ShaderResourceView *m_grassSRV;

		UINT m_vertexCount;
		UINT m_instCount;
		UINT m_maxInstCount;

		XMFLOAT2 *m_instData;
		Vertex::Grass *m_vertexData;

		int m_numGrassBB;
		Vertex::Grass *m_grassVerts;
		float m_patchRadius;
		int m_patchCountX;
		int m_patchCountZ;
		int m_patchCount;
		XMFLOAT2* m_patchPos;
		XMFLOAT2* m_visiblePatches;

		//generates and randomly orients blades in a patch
		void GeneratePatch(int bladeCount, float scale);
};

#endif    //__GRASS_H__