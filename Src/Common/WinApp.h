/**************************************************************************
 *
 * 	WinApp.h by z.keyring (c) 2013 All Rights Reserved.
 *
 *                                    --code with gVim :):
 *************************************************************************/

#ifndef __WINAPP_H__
#define __WINAPP_H__ 1

#include <windows.h>

#include "Timer.h"
#include "AppUtil.h"

class WinApp{
	
	public:
		WinApp(HINSTANCE hInstance);
		virtual ~WinApp();

		HINSTANCE AppInst() const;
		HWND      MainWnd() const;
		float     AspectRatio() const;
		int       AppWidth() const;
		int       AppHeight() const;

		int Run();

		virtual bool Init();
		virtual void OnResize();
		virtual void UpdateScene(float dt) = 0;
		virtual void RenderScene() = 0;

		virtual LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

		virtual void OnMouseDown(WPARAM, int, int){}
		virtual void OnMouseUp  (WPARAM, int, int){}
		virtual void OnMouseMove(WPARAM, int, int){}

	protected:
		bool InitMainWindow();
		bool InitDirect3D();

		void CalculateFPS();

	protected:
		HINSTANCE m_hInstance;
		HWND      m_mainWnd;

		bool      m_isMinimized;
		bool      m_isMaximized;
		bool      m_isPaused;
		bool      m_isResizing;
		UINT      m_4xMsaaQuality;

		Timer m_timer;

		ID3D11Device             *m_d3dDevice;
		ID3D11DeviceContext      *m_d3dDeviceContext;
		IDXGISwapChain           *m_swapChain;
		ID3D11Texture2D          *m_depthStencilBuffer;
		ID3D11RenderTargetView   *m_renderTargetView;
		ID3D11DepthStencilView   *m_depthStencilView;
		D3D11_VIEWPORT           m_screenViewport;

		//Derived class should set these in their own constructor
		/* to customize starting values. */

		D3D_DRIVER_TYPE  m_d3dDriverType;
		std::wstring     m_appTitle;
		bool             m_enable4XMsaa;
		int              m_appWidth;
		int              m_appHeight;
};

#endif //__WINAPP_H__

