#include "Application.h"

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	Application* app = 0;
}

Application::Application(HINSTANCE hinstance)
	: m_width(1600), m_height(900),
	m_enableMSAA(false), m_hwnd(0), m_appPaused(false),
	m_minimized(false), m_maximized(false),
	m_m4xMsaaQuality(0), m_device(0), m_immediateContext(0),
	m_SwapChain(0), m_depthStencilBuffer(0), m_RenderTargetView(0),
	m_depthStencilView(0)
{
	app = this;
}

Application::~Application()
{
	//relese recources here
}

void Application::Init()
{
	InitD3D11();


}

void Application::InitD3D11()
{
	CreateDevice();
	CheckMSAAQualityLevel();
	CreateSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();

	SetViewPort();
}

HRESULT Application::CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob)
{
	if (!fileName || !entryPoint || !shaderModel)
	{
		return E_INVALIDARG;
	}

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(fileName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, shaderModel,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

void Application::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG) 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HR(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, m_device.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf()), L"D3D11CreateDevice Failed.");

	if (m_featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
	}
}

void Application::CheckMSAAQualityLevel()
{
	HR(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_m4xMsaaQuality), L"MSAA4x Support check Failed.");
	m_enableMSAA = true;
}

void Application::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = m_width;
	sd.BufferDesc.Height = m_height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.Windowed = TRUE;

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


void Application::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer)), L"Failed to get buffer for render target view");

	HR(m_device->CreateRenderTargetView(backBuffer, 0, m_RenderTargetView.GetAddressOf()), L"Failed to create Render Target");

	ReleaseCOM(backBuffer);

}

void Application::CreateDepthStencilView()
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
	m_immediateContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	HR(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), 0, m_depthStencilView.GetAddressOf()), L"Failed to create Depth/Stencil view");

	//add to immediate context

	m_immediateContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_depthStencilView.Get());

}

void Application::SetViewPort()
{
	D3D11_VIEWPORT vp;

	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.Width = static_cast<float>(m_width);
	vp.Height = static_cast<float>(m_height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_immediateContext->RSSetViewports(1, &vp);
}


HINSTANCE Application::AppInst() const
{
	return m_hInstance;
}

HWND Application::MainWnd() const
{
	return m_hwnd;
}

float Application::AspectRatio() const
{
	return static_cast<float>(m_width) / static_cast<float> (m_height);
}

void Application::Update()
{

}

void Application::Render()
{

}