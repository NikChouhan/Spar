#pragma once

#include "WinUtil.h"
#include <string>
#include <sstream>

class Application
{
public:

	Application(HINSTANCE hinstance);
	~Application();

	void Init();
	void Update();
	void Render();
	void ShutDown();

	HINSTANCE AppInst()const;
	HWND MainWnd()const;
	float AspectRatio()const;



private:
	void InitD3D11();
	HRESULT CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob);

protected:
	void CreateDevice();
	void CheckMSAAQualityLevel();
	void CreateSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void SetViewPort();

public:
	//public member variables

	UINT m_width = 800;
	UINT m_height = 600;

private:
	HINSTANCE m_hInstance;

	HWND m_hwnd;

	bool m_appPaused; // is the application paused?
	bool m_minimized; // is the application minimized? 
	bool m_maximized; // is the application maximized?
	bool m_resizing; // are the resize bars being dragged?

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL m_featureLevel;
protected:
	wrl::ComPtr<ID3D11Device> m_device;
	wrl::ComPtr<ID3D11DeviceContext> m_immediateContext;

	bool m_enableMSAA = false;
	UINT m_m4xMsaaQuality;
	wrl::ComPtr<IDXGISwapChain> m_SwapChain;

	wrl::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

	wrl::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	wrl::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	wrl::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	D3D11_VIEWPORT mScreenViewport;


	GameTimer m_timer;


	// Window title/caption. D3DApp defaults to "D3D11 Application".
	//std::wstring m_mainWndCaption;

};