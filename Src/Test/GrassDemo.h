/****************************************************************************
 *
 * 	GrassDemo.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *
 *	Demo grass render with billboard + instance + phong lighting
 ***************************************************************************/

#ifndef __GRASSDEMO_H__
#define __GRASSDEMO_H__ 

#include "WinApp.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"
#include "Sky.h"
#include "GrassInstancing.h"

class GrassDemo : public WinApp{

	public:
		GrassDemo(HINSTANCE hInstance);
		~GrassDemo();

		bool Init();
		void OnResize();
		void UpdateScene(float dt);
		void RenderScene();

		void OnMouseDown(WPARAM btnState, int x, int y);
		void OnMouseUp(WPARAM btnState, int x, int y);
		void OnMouseMove(WPARAM btnState, int x, int y);

	private:
		Sky *m_sky;

		Grass *m_grass;

		DirectLight m_dirLights[3];

		UINT m_lightCount;

		Camera m_camera;

		POINT m_lastMousePos;
};

#endif    //__GRASSDEMO_H__
