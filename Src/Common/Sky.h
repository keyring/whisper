/****************************************************************************
 *
 * 	Sky.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 				--code with gVim :)
 ***************************************************************************/

#ifndef __SKY_H__
#define __SKY_H__ 1

#include "AppUtil.h"

class Camera;

class Sky{

	public:
		Sky(ID3D11Device *device, const std::wstring &cubeMapFilename, float skySphereRadius);
		~Sky();

		ID3D11ShaderResourceView *CreateSkySRV();

		void Draw(ID3D11DeviceContext *dc, const Camera& camera);

	private:
		Sky(const Sky& rhs);
		Sky& operator=(const Sky& rhs);
		
	private:
		ID3D11Buffer *m_VB;
		ID3D11Buffer *m_IB;

		ID3D11ShaderResourceView *m_skySRV;

		UINT m_indexCount;
};

#endif    //__SKY_H__
