/**************************************************************************
 *
 * 	WinApp.cpp by z.keyring (c) 2013 All Right Reserved.
 *
 * 					--code with gVim :)
 *************************************************************************/

#include <WindowsX.h>

#include "WinApp.h"

namespace {

	WinApp *g_winApp(NULL);
}


LRESULT CALLBACK WinAppProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	return g_winApp->WinProc(hwnd, msg, wParam, lParam);
}

WinApp::WinApp(HINSTANCE hInstance):
	m_hInstance(hInstance),
	m_mainWnd(NULL),
	m_appTitle(L"Whisper"),
	m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	m_appWidth(800),
	m_appHeight(600),
	m_isMinimized(false),
	m_isMaximized(false),
	m_isPaused(false),
	m_isResizing(false),
	m_enable4XMsaa(false),
	m_4xMsaaQuality(0),

	m_d3dDevice(NULL),
	m_d3dDeviceContext(NULL),
	m_swapChain(NULL),
	m_depthStencilBuffer(NULL),
	m_renderTargetView(NULL),
	m_depthStencilView(NULL){

	ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));

	g_winApp = this;

#if defined (DEBUG) || defined (_DEBUG)
	FILE *f(NULL);
	if(AllocConsole()){
		freopen_s(&f, "CONOUT$", "w", stdout);
	}
#endif

}

WinApp::~WinApp(){

	SafeRelease(m_depthStencilView);
	SafeRelease(m_renderTargetView);
	SafeRelease(m_depthStencilBuffer);
	SafeRelease(m_swapChain);

	if(m_d3dDeviceContext)
		m_d3dDeviceContext->ClearState();

	SafeRelease(m_d3dDeviceContext);
	SafeRelease(m_d3dDevice);
}

HINSTANCE WinApp::AppInst() const { return m_hInstance; }

HWND WinApp::MainWnd() const { return m_mainWnd; }

float WinApp::AspectRatio() const { return static_cast<float>(m_appWidth) / m_appHeight; }

int WinApp::AppWidth() const { return m_appWidth; }

int WinApp::AppHeight() const { return m_appHeight; }

bool WinApp::Init(){

	if(!InitMainWindow())
		return false;

	if(!InitDirect3D())
		return false;

	return true;
}

int WinApp::Run(){

	MSG msg = {0};

	m_timer.Reset();

	while(msg.message != WM_QUIT){
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else{
			m_timer.Tick();
			if(!m_isPaused){
				CalculateFPS();
				UpdateScene(m_timer.DeltaTime());
				RenderScene();
			}
			else{
				Sleep(200);
			}
		}
	}

	return (int)msg.wParam;
}

void WinApp::OnResize(){

	assert(m_d3dDeviceContext);
	assert(m_d3dDevice);
	assert(m_swapChain);

	SafeRelease(m_depthStencilView);
	SafeRelease(m_renderTargetView);
	SafeRelease(m_depthStencilBuffer);

	HR(m_swapChain->ResizeBuffers(1, m_appWidth, m_appHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D *backBuffer;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView));
	SafeRelease(backBuffer);

	/*  Create the depth/stencil buffer and view  */

	D3D11_TEXTURE2D_DESC depthStencilDesc;


	depthStencilDesc.Width      = m_appWidth;
	depthStencilDesc.Height     = m_appHeight;
	depthStencilDesc.MipLevels  = 1;
	depthStencilDesc.ArraySize  = 1;
	depthStencilDesc.Format     = DXGI_FORMAT_D24_UNORM_S8_UINT;

	/* MSAA 4X or 1X */
	if(m_enable4XMsaa){
		depthStencilDesc.SampleDesc.Count   = 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else{
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT; 
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags      = 0;

	HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer));
	HR(m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView));

	m_d3dDeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width    = static_cast<float>(m_appWidth);
	m_screenViewport.Height   = static_cast<float>(m_appHeight);
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_d3dDeviceContext->RSSetViewports(1, &m_screenViewport);

}

LRESULT CALLBACK WinApp::WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch(msg){
		case WM_ACTIVATE:
			if(LOWORD(wParam) == WA_INACTIVE){
				m_isPaused = true;
				m_timer.Stop();
			}
			else{
				m_isPaused = false;
				m_timer.Start();
			}
			return 0;

		case WM_SIZE:
			m_appWidth  = LOWORD(lParam);
			m_appHeight = HIWORD(lParam);

			if(m_d3dDevice){
				if(wParam == SIZE_MINIMIZED){
					m_isMinimized = true;
					m_isMaximized = false;
					m_isPaused    = true;
				}
				else if(wParam == SIZE_MAXIMIZED){
					m_isMaximized = true;
					m_isMinimized = false;
					m_isPaused    = false;

					OnResize();
				}
				else if(wParam == SIZE_RESTORED){
					if(m_isMaximized){
						m_isMaximized = false;
						m_isPaused    = false;

						OnResize();
					}
					else if(m_isMinimized){
						m_isMinimized = false;
						m_isPaused    = false;

						OnResize();
					}
					else if(m_isResizing){

					}
					else{
						OnResize();
					}
				}
			}
			return 0;

		case WM_ENTERSIZEMOVE:
			m_isPaused   = true;
			m_isResizing = true;
			m_timer.Stop();
			return 0;

		case WM_EXITSIZEMOVE:
			m_isPaused   = false;
			m_isResizing = false;
			m_timer.Start();
			OnResize();
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_GETMINMAXINFO:
			((MINMAXINFO *)lParam)->ptMinTrackSize.x = 400;
			((MINMAXINFO *)lParam)->ptMinTrackSize.y = 300;
			return 0;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool WinApp::InitMainWindow(){

	WNDCLASS wc;
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc    = WinAppProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = m_hInstance;
	wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground  = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = L"Whisper";

	if(!RegisterClass(&wc)){
		MessageBox(NULL, L"Register calss failed!", L"Error", MB_OK);
		return false;
	}

	RECT winRect = { 0, 0, m_appWidth, m_appHeight };
	AdjustWindowRect( &winRect, WS_OVERLAPPEDWINDOW, false);
	int width  = winRect.right - winRect.left;
	int height = winRect.bottom - winRect.top;

	m_mainWnd = CreateWindow(
		    L"Whisper", 
			m_appTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			NULL,
			NULL,
			m_hInstance,
			NULL);

	if(!m_mainWnd){
		MessageBox(NULL, L"Create window failed!", L"Error", MB_OK);
		return false;
	}

	ShowWindow(m_mainWnd, SW_SHOW);
	UpdateWindow(m_mainWnd);
	return true;
}

bool WinApp::InitDirect3D(){

	UINT createDeviceFlags = 0;
#if defined (DEBUG) || defined (_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[6] = {
	    D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1  };

	D3D_FEATURE_LEVEL currLevel;
	HRESULT hr = D3D11CreateDevice(
			NULL,
			m_d3dDriverType,
			NULL,
			NULL,
			featureLevels,
			6,
			D3D11_SDK_VERSION,
			&m_d3dDevice,
			&currLevel,
			&m_d3dDeviceContext);

	if(FAILED(hr)){
		MessageBox(NULL, L"D3D11Create device failed!", L"Error", MB_OK);
		return false;
	}

	if(currLevel != D3D_FEATURE_LEVEL_11_0){
		MessageBox(NULL, L" Sorry, your graphce card don't support Direct3D Feature Level 11", NULL, MB_OK);
	       return false;
	}

	HR(m_d3dDevice->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			4,
			&m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);

#if defined (DEBUG) || defined (_DEBUG)
	std::string level("UnKnown");

	switch(currLevel){
		
		case D3D_FEATURE_LEVEL_11_0:
			level = "11_0";
			break;
			
		case D3D_FEATURE_LEVEL_10_1:
			level = "10.1";
			break;

		case D3D_FEATURE_LEVEL_10_0:
			level = "10.0";
			break;

		case D3D_FEATURE_LEVEL_9_3:
			level = "9.3";
			break;

		case D3D_FEATURE_LEVEL_9_2:
			level = "9.2";
			break;

		case D3D_FEATURE_LEVEL_9_1:
			level = "9.1";
			break;
	}

	std::cout<<"Your graphics card support Direct3D "<<level<<std::endl;
	std::cout<<"4X Msaa Sample Quality level: "<<m_4xMsaaQuality<<std::endl;
#endif

	DXGI_SWAP_CHAIN_DESC scDesc = {0};
	scDesc.BufferDesc.Width  = m_appWidth;
	scDesc.BufferDesc.Height = m_appHeight;
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if(m_enable4XMsaa){
		scDesc.SampleDesc.Count   = 4;
		scDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else{
		scDesc.SampleDesc.Count   = 1;
		scDesc.SampleDesc.Quality = 0;
	}

	scDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount  = 1;
	scDesc.OutputWindow = m_mainWnd;
	scDesc.Windowed     = true;
	scDesc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Flags        = 0;

	IDXGIDevice *dxgiDevice = NULL;
	HR(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

	IDXGIAdapter *dxgiAdapter = NULL;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter)));

	IDXGIFactory *dxgiFactory = NULL;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory)));

	HR(dxgiFactory->CreateSwapChain(m_d3dDevice, &scDesc, &m_swapChain));

	SafeRelease(dxgiDevice);
	SafeRelease(dxgiAdapter);
	SafeRelease(dxgiFactory);

	OnResize();

	return true;
}

void WinApp::CalculateFPS(){

	static float timeElapsed = 0.0f;
	static int frameCount = 0;

	frameCount++;

	if(m_timer.TotalTime() - timeElapsed >= 1.0f){
		float fps = (float)frameCount;
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_appTitle << L"    "
		     << L"FPS: " << fps << L"    "
		     << L"Frame Time: " << mspf <<L" ms";
		SetWindowText(m_mainWnd, outs.str().c_str());

		frameCount = 0;
		timeElapsed += 1.0f;
	}
}
