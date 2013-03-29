/**************************************************************************
 *
 * 	BasicGeo.h by z.keyring (c) All Rights Reserved.
 *
 * 					--code with gVim :)
 *************************************************************************/

#ifndef __BASICGEO_H__
#define __BASICGEO_H__ 1

#include "AppUtil.h"

class BasicGeo{

	public:
		struct Vertex{
			Vertex(){}
			Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv):
				Position(p),
				Normal(n),
				TangentU(t),
				TexCoord(uv){}

				Vertex(	float px, float py, float pz,
					float nx, float ny, float nz,
					float tx, float ty, float tz,
					float u, float v):

					Position(px, py, pz),
					Normal(nx, ny, nz),
					TangentU(tx, ty, tz),
					TexCoord(u, v){}

				XMFLOAT3 Position;
				XMFLOAT3 Normal;
				XMFLOAT3 TangentU;
				XMFLOAT2 TexCoord;
		};

		struct MeshData{
			std::vector<Vertex> Vertices;
			std::vector<UINT> Indices;
		};

		void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);
};

#endif    //__BASICGEO_H__
