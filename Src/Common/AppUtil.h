/************************************************************************
 *
 * 	AppUtil.h by z.keyring (c) 2013 All Right Reserved.
 *
 * 					--code with gVim :)
 * *********************************************************************/

#ifndef __APPUTIL_H__
#define __APPUTIL_H__ 1

#if defined (DEBUG) || defined (_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <d3dx11.h>
#include <d3dx11Effect.h>
#include <xnamath.h>
#include <dxerr.h>
#include <assert.h>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

#if defined (DEBUG) || defined (_DEBUG)
	#ifndef HR
	#define HR(x) { HRESULT hr = (x); if(FAILED(hr)){ \
		DXTrace(__FILE__,(DWORD)__LINE__,hr, L#x, true);}}
	#endif
#else
	#ifndef HR
	#defien HR(x) (x)
	#endif
#endif

#define SafeRelease(x) { if(x) { (x)->Release(); (x) = NULL; } }
#define SafeDelete(x) { delete (x);  (x) = NULL; }

inline int KeyDown(int x) { return GetAsyncKeyState(x) & 0x8000; }

namespace COLORS{
	const XMVECTORF32 White     = { 1.0f, 1.0f, 1.0f, 1.0f };
	const XMVECTORF32 Black     = { 0.0f, 0.0f, 0.0f, 1.0f };
	const XMVECTORF32 Red       = { 1.0f, 0.0f, 0.0f, 1.0f };
	const XMVECTORF32 Green     = { 0.0f, 1.0f, 0.0f, 1.0f };
	const XMVECTORF32 Blue      = { 0.0f, 0.0f, 1.0f, 1.0f };
	const XMVECTORF32 Yellow    = { 1.0f, 1.0f, 0.0f, 1.0f };
	const XMVECTORF32 Cyan      = { 0.0f, 1.0f, 1.0f, 1.0f };
	const XMVECTORF32 Magenta   = { 1.0f, 0.0f, 1.0f, 1.0f };

	const XMVECTORF32 Silver    = { 0.75f, 0.75f, 0.75f, 1.0f };
	const XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f };
};

#endif  //__APPUTIL_H__

