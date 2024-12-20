#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>


#include "renderer.h"
#include "Model.h"
#include "Log.h"

Spar::Renderer::Renderer()
{

}

Spar::Renderer::~Renderer()
{

}

void Spar::Renderer::Init()
{
	InitWindow();
	InitD3D11();	
}

void Spar::Renderer::Submit(Model& model)
{
	// set the puny things
	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
	auto rtv = m_RenderTargetView.Get();
	m_context->OMSetRenderTargets(1, &rtv, m_depthStencilView.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_vertexLayout.Get());

	//set vs shader
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// error checking for setting the texresources

	if(!model.SetTexResources())
	{
		Log::Error("Failed to set texture resources");
		return;
	}

	model.Render();

}

void Spar::Renderer::Clear()
{
	assert(m_context);
	assert(m_SwapChain);
	SetViewPort();
	auto rtv = m_RenderTargetView.Get();
	m_context->ClearRenderTargetView(rtv, DirectX::Colors::CadetBlue);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Spar::Renderer::Present() const
{
	m_SwapChain->Present(0, 0);
}




void Spar::Renderer::InitWindow()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	else
	{
		//Create window
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);

			//Fill the surface white
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 255, 255));

			//Update the surface
			SDL_UpdateWindowSurface(window);

			Log::Info("[SDL] Window initialised");
		}
	}
}

void Spar::Renderer::InitD3D11()
{
	CreateDevice();
	//CheckMSAAQualityLevel();
	CreateSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();

	SetViewPort();
}




void Spar::Renderer::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG) 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, m_device.GetAddressOf(), &m_featureLevel, m_context.GetAddressOf());

	if (FAILED(hr))
	{
		Log::Error("Failed to create D3D11 device");
	}
	else
		Log::Info("[D3D] Device created");

	if (m_featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
	}
}

void Spar::Renderer::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = m_width;
	sd.BufferDesc.Height = m_height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.Windowed = TRUE;

	SDL_SysWMinfo wmInfo = {};
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;

	m_hwnd = hwnd;

	if (m_enableMSAA)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = m_hwnd;

	IDXGIDevice* dxgiDevice = nullptr;
	HRESULT hr = m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**> (&dxgiDevice));
	if (FAILED(hr))
	{
		Log::Error("Failed to get DXGI device");
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**> (&dxgiAdapter));
	if (FAILED(hr))
	{
		Log::Error("Failed to get DXGI adapter");
	}

	IDXGIFactory* dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if(FAILED(hr))
	{
		Log::Error("Failed to get DXGI factory");
	}

	dxgiFactory->CreateSwapChain(m_device.Get(), &sd, m_SwapChain.GetAddressOf());

	if (FAILED(hr))
	{
		Log::Error("Failed to create swap chain");
	}
	else
		Log::Info("[D3D] Swapchain initialised");

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
}


void Spar::Renderer::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer));
	if (FAILED(hr))
	{
		Log::Error("Failed to get back buffer for rtv");
		return;
	}

	if (!backBuffer)
	{
		Log::Error("No backbuffer");
	}
	else
	{
		hr = m_device->CreateRenderTargetView(backBuffer, 0, m_RenderTargetView.GetAddressOf());

		if (FAILED(hr))
		{
			Log::Error("Failed to create render target view");
		}
		else
		{
			Log::Info("[D3D] Render Target View created");
		}
	}	

	ReleaseCOM(backBuffer);
}

void Spar::Renderer::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC depthTextDesc = {};
	depthTextDesc.Width = m_width;
	depthTextDesc.Height = m_height;
	depthTextDesc.MipLevels = 1;
	depthTextDesc.ArraySize = 1;
	depthTextDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (m_enableMSAA)
	{
		depthTextDesc.SampleDesc.Count = 4;
		depthTextDesc.SampleDesc.Quality = m_m4xMsaaQuality - 1;
	}
	else
	{
		depthTextDesc.SampleDesc.Count = 1;
		depthTextDesc.SampleDesc.Quality = 0;
	}

	depthTextDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextDesc.CPUAccessFlags = 0;
	depthTextDesc.MiscFlags = 0;

	HRESULT hr = m_device->CreateTexture2D(&depthTextDesc, 0, m_depthStencilBuffer.GetAddressOf());

	if(FAILED(hr))
	{
		Log::Error("Failed to create depth stencil buffer");
	}

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf());
	if(FAILED(hr))
	{
		Log::Error("Failed to create depth stencil state");
	}

	// Set the depth stencil state
	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), 0, m_depthStencilView.GetAddressOf());

	if (FAILED(hr))
	{
		Log::Error("Failed to create depth stencil view");
	}
	else Log::Info("[D3D] Depth Stencil View created");

	//add to immediate context

	m_context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

void Spar::Renderer::SetViewPort() const
{
	D3D11_VIEWPORT vp = {};

	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.Width = static_cast<float>(m_width);
	vp.Height = static_cast<float>(m_height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_context->RSSetViewports(1, &vp);
}


float Spar::Renderer::AspectRatio() const
{
	return static_cast<float>(m_width) / static_cast<float> (m_height);
}