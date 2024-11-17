#include "renderer.h"
#include "Model.h"

Spar::Graphics::Renderer::Renderer()
{

}

Spar::Graphics::Renderer::~Renderer()
{

}

void Spar::Graphics::Renderer::Init()
{
	InitWindow();
	InitD3D11();	
}

void Spar::Graphics::Renderer::Submit(Model model)
{
	// set the puny things
	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
	auto rtv = m_RenderTargetView.Get();
	m_context->OMSetRenderTargets(1, &rtv, m_depthStencilView.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_vertexLayout.Get());
	// Set the shaders
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

void Spar::Graphics::Renderer::Clear()
{
	assert(m_context);
	assert(m_SwapChain);
	SetViewPort();
	auto rtv = m_RenderTargetView.Get();
	m_context->ClearRenderTargetView(rtv, DirectX::Colors::CadetBlue);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

void Spar::Graphics::Renderer::Present() const
{
	m_SwapChain->Present(1, 0);
}




void Spar::Graphics::Renderer::InitWindow()
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
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 255, 255, 255));

			//Update the surface
			SDL_UpdateWindowSurface(window);

			//Hack to get window to stay up
			//SDL_Event e; bool quit = false; while (quit == false) { while (SDL_PollEvent(&e)) { if (e.type == SDL_QUIT) quit = true; } }
		}
	}
}

void Spar::Graphics::Renderer::InitD3D11()
{
	CreateDevice();
	CheckMSAAQualityLevel();
	CreateSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();

	SetViewPort();
}




void Spar::Graphics::Renderer::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG) 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HR(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, m_device.GetAddressOf(), &m_featureLevel, m_context.GetAddressOf()), L"D3D11CreateDevice Failed.");

	if (m_featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
	}
}

void Spar::Graphics::Renderer::CheckMSAAQualityLevel()
{
	HR(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_m4xMsaaQuality), L"MSAA4x Support check Failed.");
	m_enableMSAA = true;
}

void Spar::Graphics::Renderer::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = m_width;
	sd.BufferDesc.Height = m_height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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
	HR((m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**> (&dxgiDevice))), L"Failed to query DXGI device");

	IDXGIAdapter* dxgiAdapter = nullptr;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**> (&dxgiAdapter)), L"Failed to Get DXGI Adapter");

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory), L"Failed to get DXGI factory pointer");

	//enumerating devices for fun

	UINT i = 0;
	IDXGIAdapter* pAdapter;
	std::vector<IDXGIAdapter*> vAdapters;
	std::wstringstream debugOutput;

	debugOutput << L"Enumerating adapters:\n";
	OutputDebugString(L"Total adapters found: ");

	while (dxgiFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);

		debugOutput << L"Adapter " << i << L": " << adapterDesc.Description << L"\n";
		OutputDebugString(adapterDesc.Description);

		vAdapters.push_back(pAdapter);
		++i;
	}


	HR(dxgiFactory->CreateSwapChain(m_device.Get(), &sd, m_SwapChain.GetAddressOf()), L"Failed to create swapchain");


	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
}


void Spar::Graphics::Renderer::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer)), L"Failed to get buffer for render target view");

	HR(m_device->CreateRenderTargetView(backBuffer, 0, m_RenderTargetView.GetAddressOf()), L"Failed to create Render Target");

	ReleaseCOM(backBuffer);

}

void Spar::Graphics::Renderer::CreateDepthStencilView()
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

	HR(m_device->CreateTexture2D(&depthTextDesc, 0, m_depthStencilBuffer.GetAddressOf()), L"Failed to create Depth/Stencil buffer");


	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	HR(m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf()), L"Failed to create depth stencil state");

	// Set the depth stencil state
	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	HR(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), 0, m_depthStencilView.GetAddressOf()), L"Failed to create Depth/Stencil view");

	//add to immediate context

	m_context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_depthStencilView.Get());

}

void Spar::Graphics::Renderer::SetViewPort() const
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


float Spar::Graphics::Renderer::AspectRatio() const
{
	return static_cast<float>(m_width) / static_cast<float> (m_height);
}