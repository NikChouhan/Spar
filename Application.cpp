#include "Application.h"


Spar::Application::Application()
{
    m_renderer = std::make_shared<Graphics::Renderer>();
    m_camera = std::make_shared<Graphics::Camera>();
}

Spar::Application::~Application()
{
    ShutDown();
}

void Spar::Application::Init()
{
    m_renderer->Init();

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

    VertexBuffer vb;

    m_renderer->m_device->CreateBuffer(&bd, &InitData, vb.Buffer.GetAddressOf());

    IndexBuffer ib;

    vb.Stride = sizeof(SimpleVertex);
    vb.Offset = 0;
    m_renderer->m_context->IASetVertexBuffers(0, 1, vb.Buffer.GetAddressOf(), &vb.Stride, &vb.Offset);

    textureCube.assets.vertexBuffer = vb.Buffer;

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

    

    m_renderer->m_device->CreateBuffer(&bd, &InitData, ib.Buffer.GetAddressOf());

    m_renderer->m_context->IASetIndexBuffer(ib.Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    textureCube.assets.indexBuffer = ib.Buffer;

    m_renderer->m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    //load the model

    const WCHAR* texturePath = L"../../../../assets/textures/seafloor.dds";
    HRESULT hr = CreateDDSTextureFromFile(m_renderer->m_device.Get(), texturePath, nullptr, textureCube.assets.textureView.GetAddressOf());

    /*const char* path = "../../../../assets/models/sponza";
    HRESULT hr = textureCube.LoadModel(path);*/

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR(m_renderer->m_device->CreateBuffer(&bd, nullptr, &textureCube.assets.constantBuffer), L"Failed to create constant buffer");

    m_world = DirectX::XMMatrixIdentity();

    // Initialize the view matrix

    m_view = m_camera->GetViewMatrix().Transpose();
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

    m_renderer->m_context->UpdateSubresource(textureCube.assets.constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    m_world = DirectX::XMMatrixRotationY(angle);
}

void Spar::Application::Render()
{
    m_renderer->Clear();

    models.push_back(textureCube);
    
    for (auto& model : models)
    {
        m_renderer->Submit(model);
    }

    m_renderer->Present();
    
}

void Spar::Application::Resize()
{

}

void Spar::Application::ShutDown()
{

}