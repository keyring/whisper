/****************************************************************************
 *
 * 	CubeMapDemo.cpp by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *
 *	Demo skydome with cube mapping
 ***************************************************************************/

#include "WinApp.h"
#include "BasicGeo.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"
#include "Sky.h"

class CubeMap : public WinApp{

	public:
		CubeMap(HINSTANCE hInstance);
		~CubeMap();

		bool Init();
		void OnResize();
		void UpdateScene(float dt);
		void RenderScene();

		void OnMouseDown(WPARAM btnState, int x, int y);
		void OnMouseUp(WPARAM btnState, int x ,int y);
		void OnMouseMove(WPARAM btnState, int x, int y);

	private:
		Sky *m_sky;

		DirectLight m_dirLights[3];

		UINT m_lightCount;

		Camera m_camera;

		POINT m_lastMousePos;
};

CubeMap::CubeMap(HINSTANCE hInstance):
	WinApp(hInstance),
	m_sky(0),
	m_lightCount(3){

	m_appTitle = L"whisper-skydome";

	m_lastMousePos.x = 0;
	m_lastMousePos.y = 0;

	m_camera.setPosition(0.0f, 2.0f, -15.0f);

	m_dirLights[0].Ambient   = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_dirLights[0].Diffuse   = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_dirLights[0].Specular  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_dirLights[0].Direction = XMFLOAT3(0.57735F, -0.57735F, 0.57735F);

	m_dirLights[1].Ambient   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dirLights[1].Diffuse   = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_dirLights[1].Specular  = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_dirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	m_dirLights[2].Ambient   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dirLights[2].Diffuse   = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_dirLights[2].Specular  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

}

CubeMap::~CubeMap(){

	SafeDelete(m_sky);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
}

bool CubeMap::Init(){

	if(!WinApp::Init())
		return false;

	//must init Effects first since InputLayouts depend on shader signatures
	Effects::InitAll(m_d3dDevice);
	InputLayouts::InitAll(m_d3dDevice);

	m_sky = new Sky(m_d3dDevice, L"Resources/Textures/cloud_cube.dds", 5000.0f);

	return true;
}

void CubeMap::OnResize(){

	WinApp::OnResize();

	m_camera.setProjParam(0.25 * XM_PI, AspectRatio(), 1.0f, 1000.0f);
}

void CubeMap::UpdateScene(float dt){

	/*
	 * Control the camera
	 */
	 if(KeyDown('W'))
		 m_camera.MoveLookVec(10.0f * dt);
	 else if(KeyDown('S'))
		 m_camera.MoveLookVec(-10.0f * dt);

	 if(KeyDown('A'))
		 m_camera.MoveRightVec(-10.0f * dt);
	 else if(KeyDown('D'))
		 m_camera.MoveRightVec(10.0f * dt);

	 if(KeyDown('Q'))
		 m_camera.MoveUpVec(10.0f * dt);
	 else if(KeyDown('E'))
		 m_camera.MoveUpVec(-10.0f * dt);

	 /*
	  * switch the number of lights
	  */
	 if(KeyDown('0'))
		 m_lightCount = 0;
	 else if(KeyDown('1'))
		 m_lightCount = 1;
	 else if(KeyDown('2'))
		 m_lightCount = 2;
	 else if(KeyDown('3'))
		 m_lightCount = 3;
}

void CubeMap::RenderScene(){

	m_d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&COLORS::Silver));
	m_d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDeviceContext->IASetInputLayout(InputLayouts::Basic);
	m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic);
	UINT offset = 0;

	m_camera.UpdateViewMat();

	XMMATRIX view  = m_camera.getViewMat();
	XMMATRIX proj  = m_camera.getProjMat();
	XMMATRIX viewproj = m_camera.getViewProjMat();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f};

	//Set Per frame constants
	Effects::BasicFX->setDirLights(m_dirLights);
	Effects::BasicFX->setEyePosVec(m_camera.getPosition());
	Effects::BasicFX->setCubeMap(m_sky->CreateSkySRV());

	m_sky->Draw(m_d3dDeviceContext, m_camera);

	//restore default states, as the SkyFX changes them in the effect file.
	m_d3dDeviceContext->RSSetState(0);
	m_d3dDeviceContext->OMSetDepthStencilState(0, 0);

	HR(m_swapChain->Present(0, 0));
}

void CubeMap::OnMouseDown(WPARAM btnState, int x, int y){

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;

	SetCapture(m_mainWnd);
}

void CubeMap::OnMouseUp(WPARAM btnState, int x, int y){

	ReleaseCapture();
}

void CubeMap::OnMouseMove(WPARAM btnState, int x, int y){

	if((btnState & MK_LBUTTON) != 0){
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_lastMousePos.y));

		m_camera.RotateRightVec(dy);
		m_camera.RotateY(dx);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){

	//Enable run-time memory check for debug builds.
#if defined (DEBUG) | defined (_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	CubeMap cubemap(hInstance);

	if(!cubemap.Init())
		return 0;

	return cubemap.Run();
}
