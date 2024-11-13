#include "Application.h"


Spar::Application::Application()
{

}

Spar::Application::~Application()
{
    ShutDown();
}

void Spar::Application::Init()
{
	m_renderer->InitWindow();
	m_renderer->InitD3D11();

    m_camera->InitAsPerspective(90, m_renderer->m_width, m_renderer->m_height);
    m_camera->SetPosition({ 0.0f, 0.0f, -2.8f });

    //shader stuff
    Shader shader;
    const WCHAR* vsShaderPath = L"../../../../assets/shaders/VS_shader.hlsl";
    const WCHAR* psshaderPath = L"../../../../assets/shaders/PS_shader.hlsl";

    shader.ProcessShaders(m_renderer, vsShaderPath, psshaderPath);

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
    m_renderer->m_context->IASetVertexBuffers(0, 1, m_renderer->m_vertexBuffer.GetAddressOf(), &stride, &offset);

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
    bd.ByteWidth = sizeof(UINT32) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;

    HR(m_renderer->m_device->CreateBuffer(&bd, &InitData, m_renderer->m_indexBuffer.GetAddressOf()), L"Failed to create Index Buffer");

    m_renderer->m_context->IASetIndexBuffer(m_renderer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    m_renderer->m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Make sure m_constantBuffer is initialized
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR(m_renderer->m_device->CreateBuffer(&bd, nullptr, &m_constantBuffer), L"Failed to create constant buffer");

    //load the texture

    const WCHAR* texturePath = L"../../../../assets/textures/seafloor.dds";

    Model model;
    model.LoadTexture(m_renderer, m_textureView,texturePath);

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

    m_view = m_camera->GetViewMatrix().Transpose();


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

void Spar::Application::Update(float dt)
{
    static float angle = 0.0f;
    angle += dt;

    m_world = DirectX::XMMatrixRotationY(angle);

    m_meshColor.x = (sinf(angle * 1.0f) + 1.0f) * 0.5f;
    m_meshColor.y = (sinf(angle * 3.0f) + 1.0f) * 0.5f;
    m_meshColor.z = (sinf(angle * 5.0f) + 1.0f) * 0.5f;
    m_meshColor.w = 1.0f;

    ConstantBuffer cb = {
        .mWorld = SM::Matrix::CreateRotationY(DirectX::XMScalarCos(angle)) * SM::Matrix::CreateRotationZ(DirectX::XMScalarSin(angle)),
        .mView = m_camera->GetViewMatrix().Transpose(),
        .mProjection = m_camera->GetProjectionMatrix().Transpose(),
        .vMeshColor = m_meshColor
    };

    m_renderer->m_context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    m_world = DirectX::XMMatrixRotationY(angle);
}

void Spar::Application::Render()
{
    m_renderer->Clear();

    for (const auto& model : models)
    {
        models
    }
}

void Spar::Application::Resize()
{

}

void Spar::Application::ShutDown()
{

}