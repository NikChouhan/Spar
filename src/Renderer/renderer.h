#pragma once

#include "WinUtil.h"
#include "Camera.h"
#include <string>
#include <sstream>
#include "Buffer.h"

namespace Spar::Graphics
{
	class Renderer
	{
	public:
		void InitD3D11();
		void InitWindow();

		void CreateDevice();
		void CheckMSAAQualityLevel();
		void CreateSwapChain();
		void CreateRenderTargetView();
		void CreateDepthStencilView();
		void SetViewPort();
		float AspectRatio()const;


	private:
		UINT createDeviceFlags = 0;
		D3D_FEATURE_LEVEL m_featureLevel;

		HWND m_hwnd;
		//The window we'll be rendering to
		SDL_Window* window = NULL;
		//The surface contained by the window
		SDL_Surface* screenSurface = NULL;
	public:
		UINT m_width = 640;
		UINT m_height = 480;

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

		 std::unique_ptr<Graphics::Camera> m_camera = std::make_unique<Graphics::Camera>();

		wrl::ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
		wrl::ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;
		wrl::ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
		wrl::ComPtr<ID3D11RasterizerState> m_rasterState = nullptr;

		wrl::ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;

	};
}