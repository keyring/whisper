/*****************************************************************************
 *
 * 	BBGrassPatch.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 ****************************************************************************/

#ifndef __BBGRASSPATCH_H__
#define __BBGRASSPATCH_H__ 1

#include "InstancedBillboard.h"

#define DEFAULT_NUM_GRASS_BB 70

struct BBGrassVertex{
	XMFLOAT3 Pos;
	XMFLOAT2 Texcoord;
};


/* geometry for single grass blade */

static BBGrassVertex grassBBVerts[] = {
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

class BBGrassPatch : public InstancedBillboard<BBGrassVertex, XMFLOAT2>{

	public:
		BBGrassPatch(int numBB = DEFAULT_NUM_GRASS_BB, int inPatchCount = defaultPatchCount, XMFLOAT2* inPatchPos = defaultPatchPos);
		~BBGrassPatch();
		HRESULT CreateDevice(ID3D11Device *device, ID3D11DeviceContext* dc, ID3DX11Effect *effect);
		void Cull( ID3D11DeviceContext* dc, float fov, XMVECTOR eyePt, XMVECTOR look);
		void DestroyDevice();
		void Generate(XMVECTOR surfaceCenter, float surfaceR, int patchBladeCount, float inPatchRadius);

	private:
		int m_numGrassBB;
		BBGrassVertex *m_grassVerts;
		float m_patchRadius;
		int m_patchCountX;
		int m_patchCountZ;
		int m_patchCount;
		XMFLOAT2* m_patchPos;
		XMFLOAT2* m_visiblePatches;

		//generates and randomly orients blades in a patch
		void GeneratePatch(int bladeCount, float scale);

};

#endif    //__BBGRASSPATCH_H__
