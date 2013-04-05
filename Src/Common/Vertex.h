/****************************************************************************
 *
 * 	Vertex.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 ***************************************************************************/

#ifndef __VERTEX_H__
#define __VERTEX_H__ 1

#include "AppUtil.h"

namespace Vertex{

	struct Basic{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Texcoord;
	};

	struct Grass{
		XMFLOAT3 Pos;
		XMFLOAT2 Texcoord;
	};
}

class InputLayoutDesc{

	public:
		static const D3D11_INPUT_ELEMENT_DESC Pos[1];
		static const D3D11_INPUT_ELEMENT_DESC Basic[3];
		static const D3D11_INPUT_ELEMENT_DESC Grass[3];
};

class InputLayouts{

	public:
		static void InitAll(ID3D11Device *device);
		static void DestroyAll();

		static ID3D11InputLayout *Pos;
		static ID3D11InputLayout *Basic;
		static ID3D11InputLayout *Grass;
};

#endif    //__VERTEX_H__
