#include "Application.h"


Spar::Application::Application()
{

}

Spar::Application::~Application()
{
	//relese recources here
}

void Spar::Application::Init()
{
	m_renderer->InitWindow();
	m_renderer->InitD3D11();

    m_renderer->m_camera->InitAsPerspective(90, m_renderer->m_width, m_renderer->m_height);
    m_renderer->m_camera->SetPosition({ 0.0f, 0.0f, -5.0f });

    ID3DBlob* pVSBlob = nullptr;
    CompileShader(L"../../../../assets/shaders/VS_shader.hlsl", "VSMain", "vs_4_0", &pVSBlob);

	HR(m_renderer->m_device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_renderer->m_vertexShader.GetAddressOf()), L"Failed to create Vertex shader");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	HR(m_renderer->m_device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_renderer->m_vertexLayout.GetAddressOf()), L"Failed to create vertex layout");

	m_renderer->m_immediateContext->IASetInputLayout(m_renderer->m_vertexLayout.Get());

	ID3DBlob* pPSBlob = nullptr;

	CompileShader(L"../../../../assets/shaders/PS_shader.hlsl", "PSMain", "ps_4_0", &pPSBlob);

	HR(m_renderer->m_device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, m_renderer->m_pixelShader.GetAddressOf()), L"Failed to create Pixel Shader");


    using namespace DirectX;
    SimpleVertex vertices[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;

    HR(m_renderer->m_device->CreateBuffer(&bd, &InitData, m_renderer->m_vertexBuffer.GetAddressOf()), L"Failed to create Vertex buffer");

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_renderer->m_immediateContext->IASetVertexBuffers(0, 1, m_renderer->m_vertexBuffer.GetAddressOf(), &stride, &offset);

    // Create index buffer
    // Create vertex buffer
    UINT32 indices[] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UINT) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;

    HR(m_renderer->m_device->CreateBuffer(&bd, &InitData, m_renderer->m_indexBuffer.GetAddressOf()), L"Failed to create Index Buffer");

    m_renderer->m_immediateContext->IASetIndexBuffer(m_renderer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    m_renderer->m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBNeverChanges);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR(m_renderer->m_device->CreateBuffer(&bd, nullptr, &m_CBNeverChanges), L"never changes buffer init");
    bd.ByteWidth = sizeof(CBChangesOnResize);
    HR(m_renderer->m_device->CreateBuffer(&bd, nullptr, &m_CBChangesOnResize), L"on resize changes buffer init");
    bd.ByteWidth = sizeof(CBChangesEveryFrame);
    HR(m_renderer->m_device->CreateBuffer(&bd, nullptr, &m_CBChangesEveryFrame), L" every frame changes buffer init");

    // 3. Make sure m_constantBuffer is initialized
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR(m_renderer->m_device->CreateBuffer(&bd, nullptr, &m_constantBuffer), L"Failed to create constant buffer");

    //load the texture

    HR(CreateDDSTextureFromFile(m_renderer->m_device.Get(), L"../../../../assets/textures/seafloor.dds", nullptr, m_textureView.GetAddressOf()), L"Failed to load texture");

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HR(m_renderer->m_device->CreateSamplerState(&sampDesc, &m_samplerLinear), L"Failed to create texture sampler");

    m_world = XMMatrixIdentity();

    // Initialize the view matrix

    m_view = m_renderer->m_camera->GetViewMatrix().Transpose();

    CBNeverChanges cbNeverChanges;
    cbNeverChanges.mView = XMMatrixTranspose(m_view);
    m_renderer->m_immediateContext->UpdateSubresource(m_CBNeverChanges.Get(), 0, nullptr, &cbNeverChanges, 0, 0);

    // Initialize the projection matrix
    m_projection = m_renderer->m_camera->GetProjectionMatrix().Transpose();

    CBChangesOnResize cbChangesOnResize;
    cbChangesOnResize.mProjection = XMMatrixTranspose(m_projection);
    m_renderer->m_immediateContext->UpdateSubresource(m_CBChangesOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);

    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthClipEnable = true;

    // Create the rasterizer state object
    HR(m_renderer->m_device->CreateRasterizerState(&rasterDesc, m_renderer->m_rasterState.GetAddressOf()), L"Failed to create rasterizer state");
}

void Spar::Application::Run()
{
    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        // Handle events
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        // Calculate delta time
        static float lastFrame = 0.0f;
        float currentFrame = SDL_GetTicks() / 1000.0f;
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update game state
        Update(deltaTime);

        // Render frame
        Render();
    }
}

void Spar::Application::Render()
{

    assert(m_renderer->m_immediateContext);
    assert(m_renderer->m_SwapChain);
    m_renderer->SetViewPort();
    auto rtv = m_renderer->m_RenderTargetView.Get();
    m_renderer->m_immediateContext->ClearRenderTargetView(rtv, DirectX::Colors::CadetBlue);
    m_renderer->m_immediateContext->ClearDepthStencilView(m_renderer->m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_renderer->m_immediateContext->OMSetDepthStencilState(m_renderer->m_depthStencilState.Get(), 1);

    m_renderer->m_immediateContext->OMSetRenderTargets(1, &rtv, m_renderer->m_depthStencilView.Get());



    // Render a Cube
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_renderer->m_immediateContext->IASetVertexBuffers(0, 1, m_renderer->m_vertexBuffer.GetAddressOf(), &stride, &offset);
    m_renderer->m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_renderer->m_immediateContext->IASetInputLayout(m_renderer->m_vertexLayout.Get());
    m_renderer->m_immediateContext->RSSetState(m_renderer->m_rasterState.Get());

    // Set the shaders
    m_renderer->m_immediateContext->VSSetShader(m_renderer->m_vertexShader.Get(), nullptr, 0);

    // set the constant buffers
    m_renderer->m_immediateContext->VSSetConstantBuffers(0, 1, m_CBNeverChanges.GetAddressOf());
    m_renderer->m_immediateContext->VSSetConstantBuffers(1, 1, m_CBChangesEveryFrame.GetAddressOf());
    m_renderer->m_immediateContext->VSSetConstantBuffers(2, 1, m_CBChangesOnResize.GetAddressOf());

    m_renderer->m_immediateContext->PSSetShader(m_renderer->m_pixelShader.Get(), nullptr, 0);

    m_renderer->m_immediateContext->VSSetConstantBuffers(1, 1, m_CBChangesEveryFrame.GetAddressOf());
    m_renderer->m_immediateContext->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
    m_renderer->m_immediateContext->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());



    //draw the Cube
    m_renderer->m_immediateContext->DrawIndexed(36, 0, 0);

    HR((m_renderer->m_SwapChain->Present(0, 0)), L"Failed to draw scene");
}

void Spar::Application::Update(float dt)
{
    static float angle = 0.0f;
    angle += dt;

    m_world = DirectX::XMMatrixRotationY(angle);

    m_meshColor.x = (sinf(angle * 1.0f) + 1.0f) * 0.5f;
    m_meshColor.y = (sinf(angle * 3.0f) + 1.0f) * 0.5f;
    m_meshColor.z = (sinf(angle * 5.0f) + 1.0f) * 0.5f;

    CBChangesEveryFrame cbEF;

    cbEF.mWorld = DirectX::XMMatrixTranspose(m_world);
    cbEF.vMeshColor = m_meshColor;

    m_renderer->m_immediateContext->UpdateSubresource(m_CBChangesEveryFrame.Get(), 0, nullptr, &cbEF, 0, 0);
}

void Spar::Application::Resize()
{
}

void Spar::Application::ShutDown()
{

}