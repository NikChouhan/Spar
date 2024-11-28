#pragma once

#include "WinUtil.h"
#include "Camera.h"
#include <string>
#include <sstream>
#include "Buffer.h"

namespace Spar { class Model; }

namespace Spar
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		void Init();
		void Submit(Model& model);
		void Clear();
		void Present() const;

		void InitD3D11();
		void InitWindow();
		void CreateDevice();
		void CheckMSAAQualityLevel();
		void CreateSwapChain();
		void CreateRenderTargetView();
		void CreateDepthStencilView();
		void SetViewPort() const;
		float AspectRatio()const;

	public:
		wrl::ComPtr<ID3D11Device> m_device;
		wrl::ComPtr<ID3D11DeviceContext> m_context;

		UINT m_width = 1000;
		UINT m_height = 600;

		//std::vector<DrawableAssets> assets;

		UINT createDeviceFlags = 0;
		D3D_FEATURE_LEVEL m_featureLevel;

		HWND m_hwnd;
		//The window we'll be rendering to
		SDL_Window* window = NULL;
		//The surface contained by the window
		SDL_Surface* screenSurface = NULL;

		bool m_enableMSAA = false;
		UINT m_m4xMsaaQuality;
		wrl::ComPtr<IDXGISwapChain> m_SwapChain;
		wrl::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
		wrl::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
		wrl::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		wrl::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

		D3D11_VIEWPORT mScreenViewport;

		wrl::ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
		wrl::ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
		wrl::ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;
	};
}