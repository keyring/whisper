/**************************************************************************
 *
 * 	AppUtil.cpp by z.keyring (c) 2013 All Right Reserved.
 *
 *					--code with gVim :)
 *************************************************************************/

#include "AppUtil.h"


bool SphereConeTest ( FXMVECTOR sphereCenter, float radius, float fov, FXMVECTOR eyePt, FXMVECTOR lookAt){

	float fSin = sinf(fov);
	float fCos = cosf(fov);
	float fInvSin = 1.0f/fSin;
	float fCosSqr = fCos*fCos;
	float fSinSqr = fSin*fSin;

	XMVECTOR vAxis = lookAt - eyePt;

	XMVECTOR vCmV =  sphereCenter - eyePt;
	XMVECTOR vD = vCmV + (radius*fInvSin)*vAxis;

	float fDSqrLen = XMVectorGetX(vD * vD);
	float fE = XMVectorGetX(vD * vAxis);

	if (fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr)	{
		fDSqrLen = XMVectorGetX(vCmV * vCmV);
		fE = -XMVectorGetX(vCmV * vAxis);
		if (fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr)	{
			float fRSqr = radius*radius;
			return fDSqrLen <= fRSqr;
		}
		return true;
	}
	return false;
}